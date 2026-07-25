//bin/sh -c 'o=${0%.c}; [ "$o" -nt "$0" ] || { ${CC:-cc} -O3 -Wall -Wextra -Werror -o "$o" "$0" || exit; }; exec "$o" "$@"' "$0" "$@"; exit
/** lastdb: one-file durable append-only tenant key/value store, no sqlite, no deps. */
#define _POSIX_C_SOURCE 200809L
#include <errno.h>
#include <fcntl.h>
#include <limits.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/uio.h>
#include <unistd.h>

#ifndef O_CLOEXEC
#define O_CLOEXEC 0
#endif

#ifndef O_DIRECTORY
#define O_DIRECTORY 0
#endif

#define MAGIC 0x3144534cU
#define OP_PUT 1
#define OP_DEL 2
#define FNV0 0xcbf29ce484222325ULL
#define FNV1 0x100000001b3ULL

typedef struct __attribute__((packed)) {
    uint32_t magic;
    uint32_t len;
    uint16_t t_len;
    uint16_t k_len;
    uint32_t v_len;
    uint8_t op;
    uint8_t pad[7];
    uint64_t key_hash;
    uint64_t check;
} Record;

typedef struct {
    uint64_t hash;
    uint64_t off1;
} Node;

static uint8_t *map_base;
static size_t map_size;
static size_t valid_size;
static Node *ht;
static uint64_t ht_cap;
static uint64_t ht_len;

static void die(const char *msg)
{
    perror(msg);
    exit(1);
}

static void diex(const char *msg)
{
    fputs(msg, stderr);
    fputc('\n', stderr);
    exit(1);
}

static uint64_t fnv_bytes(uint64_t h, const void *p, size_t n)
{
    const uint8_t *b = p;
    for (size_t i = 0; i < n; i++) {
        h ^= b[i];
        h *= FNV1;
    }
    return h;
}

static uint64_t fnv_u64(uint64_t h, uint64_t x)
{
    for (size_t i = 0; i < 8; i++) {
        h ^= (uint8_t)(x & 0xff);
        x >>= 8;
        h *= FNV1;
    }
    return h;
}

static uint64_t key_hash(const char *t, uint16_t tl, const char *k, uint16_t kl)
{
    uint64_t h = FNV0;
    h = fnv_bytes(h, t, tl);
    h = fnv_u64(h, 0);
    h = fnv_bytes(h, k, kl);
    return h ? h : 1;
}

static uint64_t rec_check(const Record *r, const char *t, const char *k, const char *v)
{
    uint64_t h = FNV0;
    h = fnv_u64(h, r->magic);
    h = fnv_u64(h, r->len);
    h = fnv_u64(h, r->t_len);
    h = fnv_u64(h, r->k_len);
    h = fnv_u64(h, r->v_len);
    h = fnv_u64(h, r->op);
    h = fnv_u64(h, r->key_hash);
    h = fnv_bytes(h, t, r->t_len);
    h = fnv_bytes(h, k, r->k_len);
    h = fnv_bytes(h, v, r->v_len);
    return h;
}

static Record *rec_at(uint64_t off)
{
    return (Record *)(void *)(map_base + off);
}

static char *rec_t(Record *r)
{
    return (char *)r + sizeof(*r);
}

static char *rec_k(Record *r)
{
    return rec_t(r) + r->t_len;
}

static char *rec_v(Record *r)
{
    return rec_k(r) + r->k_len;
}

static int rec_valid(uint64_t off)
{
    if (off > map_size || map_size - off < sizeof(Record)) {
        return 0;
    }

    Record *r = rec_at(off);
    if (r->magic != MAGIC) {
        return 0;
    }

    uint64_t body = (uint64_t)r->t_len + r->k_len + r->v_len;
    if (r->len < sizeof(Record) || r->len - sizeof(Record) != body) {
        return 0;
    }

    if (map_size - off < r->len) {
        return 0;
    }

    if (r->op != OP_PUT && r->op != OP_DEL) {
        return 0;
    }
    if (r->op == OP_DEL && r->v_len) {
        return 0;
    }

    return r->check == rec_check(r, rec_t(r), rec_k(r), rec_v(r));
}

static int key_eq(uint64_t off, const char *t, uint16_t tl, const char *k, uint16_t kl)
{
    Record *r = rec_at(off);
    if (r->t_len != tl || r->k_len != kl) {
        return 0;
    }
    return memcmp(rec_t(r), t, tl) == 0 && memcmp(rec_k(r), k, kl) == 0;
}

static void ht_grow(void)
{
    uint64_t ncap = ht_cap ? ht_cap * 2 : 4096;
    Node *nht = calloc(ncap, sizeof(*nht));
    if (!nht) {
        die("calloc");
    }

    for (uint64_t i = 0; i < ht_cap; i++) {
        if (!ht[i].off1) {
            continue;
        }

        uint64_t j = ht[i].hash & (ncap - 1);
        uint64_t dist = 0;
        uint64_t hash = ht[i].hash;
        uint64_t off1 = ht[i].off1;
        while (nht[j].off1) {
            uint64_t existing_dist = (j - nht[j].hash) & (ncap - 1);
            if (existing_dist < dist) {
                uint64_t tmp_hash = nht[j].hash;
                uint64_t tmp_off1 = nht[j].off1;
                nht[j].hash = hash;
                nht[j].off1 = off1;
                hash = tmp_hash;
                off1 = tmp_off1;
                dist = existing_dist;
            }
            j = (j + 1) & (ncap - 1);
            dist++;
        }
        nht[j].hash = hash;
        nht[j].off1 = off1;
    }

    free(ht);
    ht = nht;
    ht_cap = ncap;
}

static void ht_put(uint64_t hash, uint64_t off)
{
    if (ht_len * 2 >= ht_cap) {
        ht_grow();
    }

    Record *r = rec_at(off);
    uint64_t i = hash & (ht_cap - 1);
    uint64_t dist = 0;
    while (ht[i].off1) {
        if (ht[i].hash == hash && key_eq(ht[i].off1 - 1, rec_t(r), r->t_len, rec_k(r), r->k_len)) {
            ht[i].off1 = off + 1;
            return;
        }
        if (((i - ht[i].hash) & (ht_cap - 1)) < dist) {
            break;
        }
        i = (i + 1) & (ht_cap - 1);
        dist++;
    }

    while (ht[i].off1) {
        uint64_t existing_dist = (i - ht[i].hash) & (ht_cap - 1);
        if (existing_dist < dist) {
            uint64_t tmp_hash = ht[i].hash;
            uint64_t tmp_off1 = ht[i].off1;
            ht[i].hash = hash;
            ht[i].off1 = off + 1;
            hash = tmp_hash;
            off = tmp_off1 - 1;
            dist = existing_dist;
        }
        i = (i + 1) & (ht_cap - 1);
        dist++;
    }

    ht[i].hash = hash;
    ht[i].off1 = off + 1;
    ht_len++;
}

static Node *ht_get(const char *t, uint16_t tl, const char *k, uint16_t kl)
{
    if (!ht_cap) {
        return NULL;
    }

    uint64_t hash = key_hash(t, tl, k, kl);
    uint64_t i = hash & (ht_cap - 1);
    uint64_t dist = 0;
    while (ht[i].off1) {
        if (ht[i].hash == hash && key_eq(ht[i].off1 - 1, t, tl, k, kl)) {
            return ht + i;
        }
        if (((i - ht[i].hash) & (ht_cap - 1)) < dist) {
            break;
        }
        i = (i + 1) & (ht_cap - 1);
        dist++;
    }
    return NULL;
}

static void load_db(const char *path)
{
    valid_size = 0;
    int fd = open(path, O_RDONLY | O_CLOEXEC);
    if (fd < 0 && errno == ENOENT) {
        return;
    }
    if (fd < 0) {
        die("open");
    }

    struct stat st;
    if (fstat(fd, &st)) {
        die("fstat");
    }

    if (!st.st_size) {
        if (close(fd)) {
        die("close");
    }
        return;
    }

    map_size = (size_t)st.st_size;
    map_base = mmap(NULL, map_size, PROT_READ, MAP_PRIVATE, fd, 0);
    if (close(fd)) {
        die("close");
    }
    if (map_base == MAP_FAILED) {
        die("mmap");
    }
    (void)posix_madvise(map_base, map_size, POSIX_MADV_SEQUENTIAL);
    (void)posix_madvise(map_base, map_size, POSIX_MADV_WILLNEED);

    uint64_t off = 0;
    while (off < map_size) {
        if (!rec_valid(off)) {
            break;
        }
        Record *r = rec_at(off);
        ht_put(r->key_hash, off);
        off += r->len;
    }
    valid_size = (size_t)off;
}

static void lock_ex(int fd)
{
    struct flock fl = {0};
    fl.l_type = F_WRLCK;
    fl.l_whence = SEEK_SET;
    while (fcntl(fd, F_SETLKW, &fl)) {
        if (errno != EINTR) {
            die("fcntl lock");
        }
    }
}

static void sync_fd(int fd)
{
    if (fsync(fd)) {
        die("fsync");
    }
}

static void write_allv(int fd, struct iovec *iov, int n)
{
    while (n) {
        ssize_t got = writev(fd, iov, n);
        if (got < 0 && errno == EINTR) {
            continue;
        }
        if (got < 0) {
            die("writev");
        }
        if (!got) {
            diex("writev returned zero");
        }

        size_t done = (size_t)got;
        while (n && done >= iov[0].iov_len) {
            done -= iov[0].iov_len;
            iov++;
            n--;
        }
        if (n && done) {
            iov[0].iov_base = (char *)iov[0].iov_base + done;
            iov[0].iov_len -= done;
        }
    }
}

static void append_fd(int fd, const char *t, const char *k, const char *v, uint8_t op)
{
    size_t tl = strlen(t);
    size_t kl = strlen(k);
    size_t vl = v ? strlen(v) : 0;
    if (tl > UINT16_MAX || kl > UINT16_MAX) {
        diex("tenant/key too long");
    }
    if (vl > UINT32_MAX) {
        diex("value too long");
    }
    if (sizeof(Record) + tl + kl + vl > UINT32_MAX) {
        diex("record too long");
    }

    Record r = {0};
    r.magic = MAGIC;
    r.len = (uint32_t)(sizeof(r) + tl + kl + vl);
    r.t_len = (uint16_t)tl;
    r.k_len = (uint16_t)kl;
    r.v_len = (uint32_t)vl;
    r.op = op;
    r.key_hash = key_hash(t, r.t_len, k, r.k_len);
    r.check = rec_check(&r, t, k, v ? v : "");

    struct iovec iov[4] = {
        {&r, sizeof(r)},
        {(void *)t, tl},
        {(void *)k, kl},
        {(void *)(v ? v : ""), vl},
    };
    write_allv(fd, iov, 4);
}

static int open_lockfile(const char *path)
{
    char *lock = malloc(strlen(path) + 6);
    if (!lock) {
        die("malloc");
    }
    sprintf(lock, "%s.lock", path);

    int fd = open(lock, O_RDWR | O_CREAT | O_CLOEXEC, 0666);
    free(lock);
    if (fd < 0) {
        die("open lock");
    }
    lock_ex(fd);
    return fd;
}

static int open_append(const char *path)
{
    int fd = open(path, O_WRONLY | O_CREAT | O_APPEND | O_CLOEXEC, 0666);
    if (fd < 0) {
        die("open");
    }
    if (map_size != valid_size && ftruncate(fd, (off_t)valid_size)) {
        die("ftruncate");
    }
    return fd;
}

static int do_write(const char *path, const char *t, const char *k, const char *v, uint8_t op)
{
    int lockfd = open_lockfile(path);
    load_db(path);
    int fd = open_append(path);
    append_fd(fd, t, k, v, op);
    sync_fd(fd);
    if (close(fd)) {
        die("close");
    }
    close(lockfd);
    return 0;
}

static int do_get(const char *t, const char *k)
{
    if (strlen(t) > UINT16_MAX || strlen(k) > UINT16_MAX) {
        return 1;
    }

    Node *n = ht_get(t, (uint16_t)strlen(t), k, (uint16_t)strlen(k));
    if (!n) {
        return 1;
    }

    Record *r = rec_at(n->off1 - 1);
    if (r->op == OP_DEL) {
        return 1;
    }

    if (r->v_len && fwrite(rec_v(r), 1, r->v_len, stdout) != r->v_len) {
        die("fwrite");
    }
    putchar('\n');
    return 0;
}

static int value_eq(const char *t, const char *k, const char *v)
{
    if (strlen(t) > UINT16_MAX || strlen(k) > UINT16_MAX) {
        return 0;
    }

    Node *n = ht_get(t, (uint16_t)strlen(t), k, (uint16_t)strlen(k));
    if (!n) {
        return 0;
    }

    Record *r = rec_at(n->off1 - 1);
    if (r->op == OP_DEL || r->v_len != strlen(v)) {
        return 0;
    }

    return memcmp(rec_v(r), v, r->v_len) == 0;
}

static int do_putnx(const char *path, const char *t, const char *k, const char *v)
{
    int lockfd = open_lockfile(path);
    load_db(path);
    if (strlen(t) > UINT16_MAX || strlen(k) > UINT16_MAX) {
        close(lockfd);
        diex("tenant/key too long");
    }

    Node *n = ht_get(t, (uint16_t)strlen(t), k, (uint16_t)strlen(k));
    if (n) {
        Record *r = rec_at(n->off1 - 1);
        if (r->op != OP_DEL) {
            close(lockfd);
            return 1;
        }
    }

    int fd = open_append(path);
    append_fd(fd, t, k, v, OP_PUT);
    sync_fd(fd);
    if (close(fd)) {
        die("close");
    }
    close(lockfd);
    puts(v);
    return 0;
}

static int do_cas(const char *path, const char *t, const char *k, const char *old, const char *new)
{
    int lockfd = open_lockfile(path);
    load_db(path);
    if (!value_eq(t, k, old)) {
        close(lockfd);
        return 1;
    }

    int fd = open_append(path);
    append_fd(fd, t, k, new, OP_PUT);
    sync_fd(fd);
    if (close(fd)) {
        die("close");
    }
    close(lockfd);
    puts(new);
    return 0;
}

static int do_delif(const char *path, const char *t, const char *k, const char *v)
{
    int lockfd = open_lockfile(path);
    load_db(path);
    if (!value_eq(t, k, v)) {
        close(lockfd);
        return 1;
    }

    int fd = open_append(path);
    append_fd(fd, t, k, NULL, OP_DEL);
    sync_fd(fd);
    if (close(fd)) {
        die("close");
    }
    close(lockfd);
    puts(v);
    return 0;
}

static long long parse_i64(const char *s)
{
    char *end = NULL;
    errno = 0;
    long long x = strtoll(s, &end, 10);
    if (!*s || errno || *end) {
        diex("invalid integer");
    }
    return x;
}

static long long current_int(const char *t, const char *k)
{
    if (strlen(t) > UINT16_MAX || strlen(k) > UINT16_MAX) {
        return 0;
    }

    Node *n = ht_get(t, (uint16_t)strlen(t), k, (uint16_t)strlen(k));
    if (!n) {
        return 0;
    }

    Record *r = rec_at(n->off1 - 1);
    if (r->op == OP_DEL) {
        return 0;
    }
    if (r->v_len >= 64) {
        diex("stored integer too long");
    }

    char buf[64] = {0};
    memcpy(buf, rec_v(r), r->v_len);
    return parse_i64(buf);
}

static int do_incr(const char *path, const char *t, const char *k, const char *delta)
{
    int lockfd = open_lockfile(path);
    load_db(path);
    long long cur = current_int(t, k);
    long long d = parse_i64(delta);
    if ((d > 0 && cur > LLONG_MAX - d) || (d < 0 && cur < LLONG_MIN - d)) {
        close(lockfd);
        diex("integer overflow");
    }

    int fd = open_append(path);
    long long next = cur + d;
    char val[64];
    snprintf(val, sizeof(val), "%lld", next);
    append_fd(fd, t, k, val, OP_PUT);
    sync_fd(fd);
    if (close(fd)) {
        die("close");
    }
    close(lockfd);
    printf("%lld\n", next);
    return 0;
}

static int do_take(const char *path, const char *t, const char *k)
{
    int lockfd = open_lockfile(path);
    load_db(path);
    long long cur = current_int(t, k);
    if (cur <= 0) {
        close(lockfd);
        return 1;
    }

    char val[64];
    snprintf(val, sizeof(val), "%lld", cur - 1);
    int fd = open_append(path);
    append_fd(fd, t, k, val, OP_PUT);
    sync_fd(fd);
    if (close(fd)) {
        die("close");
    }
    close(lockfd);
    puts(val);
    return 0;
}

static unsigned long long parse_u64(const char *s)
{
    char *end = NULL;
    errno = 0;
    unsigned long long x = strtoull(s, &end, 10);
    if (!*s || errno || *end) {
        diex("invalid offset");
    }
    return x;
}

static int do_verify(const char *path)
{
    load_db(path);
    uint64_t records = 0;
    uint64_t puts = 0;
    uint64_t dels = 0;

    for (uint64_t off = 0; off < valid_size;) {
        Record *r = rec_at(off);
        records++;
        puts += r->op == OP_PUT;
        dels += r->op == OP_DEL;
        off += r->len;
    }

    uint64_t live_keys = 0;
    for (uint64_t i = 0; i < ht_cap; i++) {
        if (ht[i].off1) {
            live_keys += rec_at(ht[i].off1 - 1)->op != OP_DEL;
        }
    }

    printf("file_bytes\t%llu\n", (unsigned long long)map_size);
    printf("valid_bytes\t%llu\n", (unsigned long long)valid_size);
    printf("bad_bytes\t%llu\n", (unsigned long long)(map_size - valid_size));
    printf("records\t%llu\n", (unsigned long long)records);
    printf("puts\t%llu\n", (unsigned long long)puts);
    printf("dels\t%llu\n", (unsigned long long)dels);
    printf("keys\t%llu\n", (unsigned long long)ht_len);
    printf("live_keys\t%llu\n", (unsigned long long)live_keys);
    if (map_size != valid_size) {
        printf("bad_offset\t%llu\n", (unsigned long long)valid_size);
        return 1;
    }
    return 0;
}

static int do_tail(const char *path, const char *start)
{
    load_db(path);
    uint64_t off = start ? (uint64_t)parse_u64(start) : 0;
    if (off > valid_size) {
        diex("offset past valid log");
    }

    while (off < valid_size) {
        if (!rec_valid(off)) {
            diex("offset is not a record boundary");
        }
        Record *r = rec_at(off);
        uint64_t next = off + r->len;
        printf("%llu\t%llu\t%s\t", (unsigned long long)next, (unsigned long long)off, r->op == OP_PUT ? "put" : "del");
        fwrite(rec_t(r), 1, r->t_len, stdout);
        putchar('\t');
        fwrite(rec_k(r), 1, r->k_len, stdout);
        putchar('\t');
        if (r->op == OP_PUT) {
            fwrite(rec_v(r), 1, r->v_len, stdout);
        }
        putchar('\n');
        off = next;
    }
    return 0;
}

typedef struct {
    uint64_t off;
    char *key;
    uint16_t k_len;
} ScanRow;

static int scan_row_cmp(const void *pa, const void *pb)
{
    const ScanRow *a = pa;
    const ScanRow *b = pb;
    size_t n = a->k_len < b->k_len ? a->k_len : b->k_len;
    int c = memcmp(a->key, b->key, n);
    if (c) {
        return c;
    }
    return (a->k_len > b->k_len) - (a->k_len < b->k_len);
}

static int do_scan(const char *t, const char *prefix)
{
    if (!ht_cap) {
        return 0;
    }

    size_t tl = strlen(t);
    size_t pl = prefix ? strlen(prefix) : 0;
    if (tl > UINT16_MAX || pl > UINT16_MAX) {
        return 0;
    }

    ScanRow *rows = NULL;
    uint64_t n = 0;
    uint64_t cap = 0;
    for (uint64_t i = 0; i < ht_cap; i++) {
        if (!ht[i].off1) {
            continue;
        }

        Record *r = rec_at(ht[i].off1 - 1);
        if (r->op == OP_DEL || r->t_len != tl) {
            continue;
        }
        if (memcmp(rec_t(r), t, tl)) {
            continue;
        }
        if (pl && (r->k_len < pl || memcmp(rec_k(r), prefix, pl))) {
            continue;
        }
        if (n == cap) {
            cap = cap ? cap * 2 : 1024;
            ScanRow *p = realloc(rows, cap * sizeof(*rows));
            if (!p) {
                die("realloc");
            }
            rows = p;
        }
        rows[n++] = (ScanRow){ht[i].off1 - 1, rec_k(r), r->k_len};
    }

    qsort(rows, n, sizeof(*rows), scan_row_cmp);
    for (uint64_t i = 0; i < n; i++) {
        Record *r = rec_at(rows[i].off);
        fwrite(rows[i].key, 1, rows[i].k_len, stdout);
        putchar('\t');
        fwrite(rec_v(r), 1, r->v_len, stdout);
        putchar('\n');
    }

    free(rows);
    return 0;
}

static void write_all(int fd, const void *p, size_t n)
{
    const char *s = p;
    while (n) {
        ssize_t got = write(fd, s, n);
        if (got < 0 && errno == EINTR) {
            continue;
        }
        if (got < 0) {
            die("write");
        }
        if (!got) {
            diex("write returned zero");
        }
        s += got;
        n -= (size_t)got;
    }
}

static void fsync_parent(const char *path)
{
    char *dir = strdup(path);
    if (!dir) {
        die("strdup");
    }

    char *slash = strrchr(dir, '/');
    if (!slash) {
        strcpy(dir, ".");
    } else if (slash == dir) {
        slash[1] = 0;
    } else {
        *slash = 0;
    }

    int fd = open(dir, O_RDONLY | O_DIRECTORY | O_CLOEXEC);
    if (fd >= 0) {
        sync_fd(fd);
        if (close(fd)) {
        die("close");
    }
    }
    free(dir);
}

static int do_compact(const char *path)
{
    int lockfd = open_lockfile(path);
    load_db(path);

    char *tmp = malloc(strlen(path) + 5);
    if (!tmp) {
        die("malloc");
    }
    sprintf(tmp, "%s.tmp", path);

    int fd = open(tmp, O_WRONLY | O_CREAT | O_TRUNC | O_CLOEXEC, 0666);
    if (fd < 0) {
        die("open tmp");
    }

    for (uint64_t off = 0; off < valid_size;) {
        Record *r = rec_at(off);
        Node *n = ht_get(rec_t(r), r->t_len, rec_k(r), r->k_len);
        if (n && (n->off1 - 1) == off && r->op != OP_DEL) {
            write_all(fd, r, r->len);
        }
        off += r->len;
    }

    sync_fd(fd);
    if (close(fd)) {
        die("close");
    }
    if (rename(tmp, path)) {
        die("rename");
    }
    fsync_parent(path);
    close(lockfd);
    free(tmp);
    return 0;
}

static int do_batch(const char *path, const char *t)
{
    int lockfd = open_lockfile(path);
    load_db(path);
    int fd = open_append(path);
    char *line = NULL;
    size_t cap = 0;

    for (;;) {
        ssize_t n = getline(&line, &cap, stdin);
        if (n < 0) {
            break;
        }
        if (n && line[n - 1] == '\n') {
            line[n - 1] = 0;
        }

        char *tab = strchr(line, '\t');
        if (!tab) {
            diex("batch line needs key<TAB>value");
        }

        *tab = 0;
        append_fd(fd, t, line, tab + 1, OP_PUT);
    }

    free(line);
    sync_fd(fd);
    if (close(fd)) {
        die("close");
    }
    close(lockfd);
    return 0;
}

static void usage(const char *prog)
{
    fprintf(stderr, "usage: %s DB CMD ...\n", prog);
    fputs("  put TENANT KEY VALUE\n", stderr);
    fputs("  putnx TENANT KEY VALUE\n", stderr);
    fputs("  cas TENANT KEY OLD NEW\n", stderr);
    fputs("  get TENANT KEY\n", stderr);
    fputs("  del TENANT KEY\n", stderr);
    fputs("  delif TENANT KEY VALUE\n", stderr);
    fputs("  scan TENANT [PREFIX]\n", stderr);
    fputs("  tail [OFFSET]\n", stderr);
    fputs("  verify\n", stderr);
    fputs("  incr TENANT KEY DELTA\n", stderr);
    fputs("  take TENANT KEY\n", stderr);
    fputs("  batch TENANT     # stdin: key<TAB>value\n", stderr);
    fputs("  compact\n", stderr);
    exit(2);
}

int main(int argc, char **argv)
{
    if (argc < 3) {
        usage(argv[0]);
    }

    const char *db = argv[1];
    const char *cmd = argv[2];

    if (!strcmp(cmd, "put") && argc == 6) {
        return do_write(db, argv[3], argv[4], argv[5], OP_PUT);
    }
    if (!strcmp(cmd, "putnx") && argc == 6) {
        return do_putnx(db, argv[3], argv[4], argv[5]);
    }
    if (!strcmp(cmd, "cas") && argc == 7) {
        return do_cas(db, argv[3], argv[4], argv[5], argv[6]);
    }
    if (!strcmp(cmd, "del") && argc == 5) {
        return do_write(db, argv[3], argv[4], NULL, OP_DEL);
    }
    if (!strcmp(cmd, "delif") && argc == 6) {
        return do_delif(db, argv[3], argv[4], argv[5]);
    }
    if (!strcmp(cmd, "get") && argc == 5) {
        load_db(db);
        return do_get(argv[3], argv[4]);
    }
    if (!strcmp(cmd, "scan") && (argc == 4 || argc == 5)) {
        load_db(db);
        return do_scan(argv[3], argc == 5 ? argv[4] : NULL);
    }
    if (!strcmp(cmd, "tail") && (argc == 3 || argc == 4)) {
        return do_tail(db, argc == 4 ? argv[3] : NULL);
    }
    if (!strcmp(cmd, "verify") && argc == 3) {
        return do_verify(db);
    }
    if (!strcmp(cmd, "incr") && argc == 6) {
        return do_incr(db, argv[3], argv[4], argv[5]);
    }
    if (!strcmp(cmd, "take") && argc == 5) {
        return do_take(db, argv[3], argv[4]);
    }
    if (!strcmp(cmd, "batch") && argc == 4) {
        return do_batch(db, argv[3]);
    }
    if (!strcmp(cmd, "compact") && argc == 3) {
        return do_compact(db);
    }

    usage(argv[0]);
}
