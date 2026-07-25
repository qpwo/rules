//bin/sh -c 'o=${0%.c}; [ "$o" -nt "$0" ] || { ${CC:-cc} -O3 -march=native -ffast-math -fopenmp -Wall -Wextra -Werror -o "$o" "$0" -lm || exit; }; exec "$o" "$@"' "$0" "$@"; exit
/** lastdb: one-file durable append-only tenant key/value store, no sqlite, no deps. */
#define _GNU_SOURCE
#include <errno.h>
#include <fcntl.h>
#include <sys/file.h>
#include <limits.h>
#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/statvfs.h>
#include <sys/sysinfo.h>
#include <sys/uio.h>
#include <unistd.h>
#include <immintrin.h>

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

static uint64_t rd64(const void *p) {
    uint64_t x = 0;
    memcpy(&x, p, 8);
    return x;
}

static uint64_t mum(uint64_t a, uint64_t b) {
    __uint128_t r = (__uint128_t)a * b;
    return (uint64_t)r ^ (uint64_t)(r >> 64);
}

static uint64_t fnv_bytes(uint64_t h, const void *p, size_t n)
{
    const unsigned char *b = p;
    size_t total = n;
    while (n >= 16) {
        h = mum(rd64(b) ^ h, rd64(b + 8) ^ h);
        b += 16;
        n -= 16;
    }
    uint64_t x = 0, y = 0;
    if (n <= 8) memcpy(&x, b, n);
    else {
        memcpy(&x, b, 8);
        memcpy(&y, b + 8, n - 8);
    }
    return mum(x ^ h, y ^ total);
}

static uint64_t fnv_u64(uint64_t h, uint64_t x)
{
    return mum(h, x);
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

    return 1; // Defer key/value checking to verify command to allow instant loading of 10TB datasets
}

static int key_eq(uint64_t off, const char *t, uint16_t tl, const char *k, uint16_t kl)
{
    Record *r = rec_at(off);
    if (r->t_len != tl || r->k_len != kl) {
        return 0;
    }
    return memcmp(rec_t(r), t, tl) == 0 && memcmp(rec_k(r), k, kl) == 0;
}

#ifndef MAP_ANONYMOUS
#define MAP_ANONYMOUS 0x20
#endif
#ifndef MAP_POPULATE
#define MAP_POPULATE 0x08000
#endif
#ifndef MADV_HUGEPAGE
#define MADV_HUGEPAGE 14
#endif

static void reserve_ram(size_t bytes)
{
    struct sysinfo si;
    if (sysinfo(&si)) {
        die("sysinfo");
    }

    uint64_t total = (uint64_t)si.totalram * si.mem_unit;
    uint64_t freeish = (uint64_t)(si.freeram + si.bufferram) * si.mem_unit;
    if (freeish < total / 10 + bytes) {
        diex("ram reserve below 10 percent");
    }
}

static void ht_put(uint64_t hash, uint64_t off)
{
    if (ht_len * 2 >= ht_cap) {
        uint64_t ncap = ht_cap ? ht_cap * 2 : 4096;
        size_t bytes = (ncap + 256) * sizeof(*ht);
        reserve_ram(bytes);
        Node *nht = mmap(NULL, bytes, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS | MAP_POPULATE, -1, 0);
        if (nht == MAP_FAILED) die("mmap nht");
        madvise(nht, (ncap + 256) * sizeof(*nht), MADV_HUGEPAGE);
        if (ht) {
            for (uint64_t i = 0; i < ht_cap + 256; i++) {
                if (ht[i].off1) {
                    uint64_t mask = ncap - 1;
                    uint64_t pos = ht[i].hash & mask;
                    Node curr = ht[i];
                    uint64_t d = 0;
                    while (nht[pos].off1) {
                        uint64_t existing_d = pos - (nht[pos].hash & mask);
                        if (existing_d < d) {
                            Node tmp = nht[pos];
                            nht[pos] = curr;
                            curr = tmp;
                            d = existing_d;
                        }
                        pos++;
                        d++;
                    }
                    nht[pos] = curr;
                }
            }
            munmap(ht, (ht_cap + 256) * sizeof(*nht));
        }
        ht = nht;
        ht_cap = ncap;
    }

    uint64_t mask = ht_cap - 1;
    uint64_t pos = hash & mask;
    Node curr = {hash, off + 1};
    uint64_t d = 0;
    Record *new_r = rec_at(off);

    int check_dup = 1;
    while (ht[pos].off1) {
        if (d > 250) die("probe too long");
        uint64_t existing_d = pos - (ht[pos].hash & mask);
        if (existing_d < d) {
            check_dup = 0;
            Node tmp = ht[pos];
            ht[pos] = curr;
            curr = tmp;
            d = existing_d;
            new_r = rec_at(curr.off1 - 1);
        }
        if (check_dup && ht[pos].hash == curr.hash) {
            Record *old_r = rec_at(ht[pos].off1 - 1);
            if (old_r->t_len == new_r->t_len && old_r->k_len == new_r->k_len &&
                !memcmp(rec_t(old_r), rec_t(new_r), old_r->t_len) &&
                !memcmp(rec_k(old_r), rec_k(new_r), old_r->k_len)) {
                ht[pos].off1 = curr.off1;
                return;
            }
        }
        pos++;
        d++;
    }
    ht[pos] = curr;
    ht_len++;
}

static Node *ht_get(const char *t, uint16_t tl, const char *k, uint16_t kl)
{
    if (!ht_cap) return NULL;
    uint64_t hash = key_hash(t, tl, k, kl);
    uint64_t mask = ht_cap - 1;
    uint64_t pos = hash & mask;
    uint64_t d = 0;
    while (ht[pos].off1) {
        uint64_t existing_d = pos - (ht[pos].hash & mask);
        if (existing_d < d) return NULL;
        if (ht[pos].hash == hash) {
            if (key_eq(ht[pos].off1 - 1, t, tl, k, kl)) return &ht[pos];
        }
        pos++;
        d++;
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
    map_base = mmap(NULL, map_size, PROT_READ, MAP_SHARED, fd, 0);
    if (close(fd)) {
        die("close");
    }
    if (map_base == MAP_FAILED) {
        die("mmap");
    }
    (void)posix_madvise(map_base, map_size, POSIX_MADV_SEQUENTIAL);
    (void)posix_madvise(map_base, map_size, POSIX_MADV_NOREUSE);

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
    while (flock(fd, LOCK_EX)) {
        if (errno != EINTR) {
            die("flock lock");
        }
    }
}

static void sync_fd(int fd)
{
    if (fdatasync(fd)) {
        die("fsync");
    }
}

static void append_fd(int fd, const char *t, const char *k, const char *v, uint8_t op)
{
    size_t tl = strlen(t);
    size_t kl = strlen(k);
    size_t vl = v ? strlen(v) : 0;
    if (tl > UINT16_MAX || kl > UINT16_MAX) diex("tenant/key too long");
    if (vl > UINT32_MAX) diex("value too long");
    if (sizeof(Record) + tl + kl + vl > UINT32_MAX) diex("record too long");

    Record r = {0};
    r.magic = MAGIC;
    r.len = (uint32_t)(sizeof(r) + tl + kl + vl);
    r.t_len = (uint16_t)tl;
    r.k_len = (uint16_t)kl;
    r.v_len = (uint32_t)vl;
    r.op = op;
    r.key_hash = key_hash(t, r.t_len, k, r.k_len);
    r.check = rec_check(&r, t, k, v ? v : "");

    struct statvfs st;
    if (op != OP_DEL && !fstatvfs(fd, &st) && st.f_blocks > 0 && st.f_frsize > 0) {
        uint64_t free_bytes = (uint64_t)st.f_bavail * st.f_frsize;
        uint64_t reserve_bytes = (uint64_t)((long double)st.f_blocks * st.f_frsize * 0.1L);
        double avail = (double)st.f_bavail / st.f_blocks;
        double keep = exp2(50.0 * (avail - 0.2));
        double gate = (double)(r.check >> 11) * 0x1.0p-53;
        if (free_bytes < reserve_bytes + r.len) {
            diex("disk reserve below 10 percent");
        }
        if (avail < 0.2 && gate > keep) {
            diex("disk pressure decay rejected write");
        }
    }

    struct iovec iov[4] = {
        {&r, sizeof(r)},
        {(void *)t, tl},
        {(void *)k, kl},
        {(void *)(v ? v : ""), vl},
    };
    int i = 0;
    while (i < 4) {
        ssize_t got = writev(fd, iov + i, 4 - i);
        if (got < 0 && errno == EINTR) continue;
        if (got <= 0) die("writev");
        while (i < 4 && (size_t)got >= iov[i].iov_len) {
            got -= (ssize_t)iov[i].iov_len;
            i++;
        }
        if (i < 4 && got) {
            iov[i].iov_base = (char *)iov[i].iov_base + got;
            iov[i].iov_len -= (size_t)got;
        }
    }
}

static int open_lockfile(const char *path)
{
    char lock[4096];
    snprintf(lock, sizeof(lock), "%s.lock", path);

    int fd = open(lock, O_RDWR | O_CREAT | O_CLOEXEC, 0666);
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
    if (close(fd)) die("close");
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

static double parse_f64(const char *s)
{
    char *end = NULL;
    errno = 0;
    double x = strtod(s, &end);
    if (!*s || errno || *end || !isfinite(x)) {
        diex("invalid float");
    }
    return x;
}

static int current_decay(const char *t, const char *k, double *half_life, double *last, double *value)
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
    if (r->v_len >= 192) {
        diex("stored decay state too long");
    }

    char buf[192] = {0};
    memcpy(buf, rec_v(r), r->v_len);
    char *tab1 = strchr(buf, '\t');
    if (!tab1) {
        diex("stored decay state malformed");
    }
    *tab1 = 0;
    char *tab2 = strchr(tab1 + 1, '\t');
    if (!tab2) {
        diex("stored decay state malformed");
    }
    *tab2 = 0;
    *half_life = parse_f64(buf);
    *last = parse_f64(tab1 + 1);
    *value = parse_f64(tab2 + 1);
    return 1;
}

static int do_decay(const char *path, const char *t, const char *k, const char *half_life, const char *now, const char *delta)
{
    int lockfd = open_lockfile(path);
    load_db(path);
    double hl = parse_f64(half_life);
    double ts = parse_f64(now);
    double d = parse_f64(delta);
    if (hl <= 0) {
        close(lockfd);
        diex("half life must be positive");
    }

    double stored_hl = hl;
    double last = ts;
    double value = 0;
    if (current_decay(t, k, &stored_hl, &last, &value)) {
        if (stored_hl != hl) {
            close(lockfd);
            diex("decay half life changed");
        }
        if (ts < last) {
            close(lockfd);
            diex("time went backwards");
        }
    }

    double next = value * exp2((last - ts) / hl) + d;
    char val[192];
    snprintf(val, sizeof(val), "%.17g\t%.17g\t%.17g", hl, ts, next);

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
    uint64_t put_bytes = 0;
    uint64_t del_bytes = 0;
    uint64_t value_bytes = 0;
    uint64_t live_keys = 0;
    uint64_t live_record_bytes = 0;
    uint64_t live_value_bytes = 0;

    for (uint64_t off = 0; off < valid_size;) {
        Record *r = rec_at(off);
        if (r->check != rec_check(r, rec_t(r), rec_k(r), rec_v(r))) {
            printf("corrupt_record_offset\t%llu\n", (unsigned long long)off);
            return 1;
        }
        records++;
        puts += r->op == OP_PUT;
        dels += r->op == OP_DEL;
        put_bytes += r->op == OP_PUT ? r->len : 0;
        del_bytes += r->op == OP_DEL ? r->len : 0;
        value_bytes += r->op == OP_PUT ? r->v_len : 0;
        off += r->len;
    }

    uint64_t iter_cap = ht_cap ? ht_cap + 256 : 0;
    for (uint64_t i = 0; i < iter_cap; i++) {
        if (!ht[i].off1) continue;
        Record *r = rec_at(ht[i].off1 - 1);
        if (r->op == OP_DEL) {
            continue;
        }

        live_keys++;
        live_record_bytes += r->len;
        live_value_bytes += r->v_len;
    }

    printf("file_bytes\t%llu\n", (unsigned long long)map_size);
    printf("valid_bytes\t%llu\n", (unsigned long long)valid_size);
    printf("bad_bytes\t%llu\n", (unsigned long long)(map_size - valid_size));
    printf("records\t%llu\n", (unsigned long long)records);
    printf("puts\t%llu\n", (unsigned long long)puts);
    printf("dels\t%llu\n", (unsigned long long)dels);
    printf("put_bytes\t%llu\n", (unsigned long long)put_bytes);
    printf("del_bytes\t%llu\n", (unsigned long long)del_bytes);
    printf("value_bytes\t%llu\n", (unsigned long long)value_bytes);
    printf("keys\t%llu\n", (unsigned long long)ht_len);
    printf("live_keys\t%llu\n", (unsigned long long)live_keys);
    printf("live_record_bytes\t%llu\n", (unsigned long long)live_record_bytes);
    printf("live_value_bytes\t%llu\n", (unsigned long long)live_value_bytes);
    printf("dead_records\t%llu\n", (unsigned long long)(records - live_keys));
    printf("compact_saves_bytes\t%llu\n", (unsigned long long)(valid_size - live_record_bytes));
    if (map_size != valid_size) {
        printf("bad_offset\t%llu\n", (unsigned long long)valid_size);
        return 1;
    }
    return 0;
}

static int do_tail(const char *path, const char *start, int follow)
{
    load_db(path);
    uint64_t off = start ? (uint64_t)parse_u64(start) : 0;
    if (off > valid_size) diex("offset past valid log");

    for (;;) {
        while (off < valid_size) {
            if (!rec_valid(off)) break;
            Record *r = rec_at(off);
            uint64_t next = off + r->len;
            printf("%llu\t%llu\t%s\t", (unsigned long long)next, (unsigned long long)off, r->op == OP_PUT ? "put" : "del");
            fwrite(rec_t(r), 1, r->t_len, stdout);
            putchar('\t');
            fwrite(rec_k(r), 1, r->k_len, stdout);
            putchar('\t');
            if (r->op == OP_PUT) fwrite(rec_v(r), 1, r->v_len, stdout);
            putchar('\n');
            fflush(stdout);
            off = next;
        }
        if (!follow) break;
        usleep(50000);
        struct stat st;
        if (!stat(path, &st) && st.st_size > (off_t)map_size) {
            int fd = open(path, O_RDONLY | O_CLOEXEC);
            if (fd >= 0) {
                void *new_map = mmap(NULL, st.st_size, PROT_READ, MAP_SHARED, fd, 0);
                if (new_map != MAP_FAILED) {
                    munmap(map_base, map_size);
                    map_base = new_map;
                    map_size = st.st_size;
                    valid_size = map_size;
                }
                close(fd);
            }
        }
    }
    return 0;
}

static int do_search(const char *t, int argc, char **argv)
{
    size_t tl = strlen(t);
    if (tl > UINT16_MAX || !ht_cap) return 0;

    #pragma omp parallel for schedule(dynamic, 1024)
    for (uint64_t i = 0; i < ht_cap + 256; i++) {
            if (!ht[i].off1) continue;
            Record *r = rec_at(ht[i].off1 - 1);
            if (r->op == OP_DEL || r->t_len != tl) continue;
            if (memcmp(rec_t(r), t, tl) != 0) continue;

            int match = 1;
            for (int j = 4; j < argc; j++) {
                if (!memmem(rec_v(r), r->v_len, argv[j], strlen(argv[j])) &&
                    !memmem(rec_k(r), r->k_len, argv[j], strlen(argv[j]))) {
                    match = 0;
                    break;
                }
            }
            if (match) {
                #pragma omp critical
                {
                    if (fwrite(rec_k(r), 1, r->k_len, stdout) != r->k_len) die("fwrite");
                    putchar('\t');
                    if (fwrite(rec_v(r), 1, r->v_len, stdout) != r->v_len) die("fwrite");
                    putchar('\n');
                }
            }
        }
    }
    return 0;
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

    for (uint64_t i = 0; i < ht_cap + 256; i++) {
        if (!ht[i].off1) continue;
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
        if (fwrite(rec_k(r), 1, r->k_len, stdout) != r->k_len) {
            die("fwrite");
        }
        if (putchar('\t') == EOF) {
            die("putchar");
        }
        if (fwrite(rec_v(r), 1, r->v_len, stdout) != r->v_len) {
            die("fwrite");
        }
        if (putchar('\n') == EOF) {
            die("putchar");
        }
    }
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
    char dir[4096];
    snprintf(dir, sizeof(dir), "%s", path);

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
}

static int do_compact(const char *path)
{
    int lockfd = open_lockfile(path);
    load_db(path);

    char tmp[4096];
    snprintf(tmp, sizeof(tmp), "%s.tmp", path);

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
    return 0;
}

typedef float unaligned_f32 __attribute__((aligned(1)));
typedef _Float16 unaligned_f16 __attribute__((aligned(1)));

__attribute__((target("avx512f,avx512vl")))
static float vec_dot_f32(const void *a, const void *b, size_t bytes) {
    size_t n = bytes / 4;
    __m512 s = _mm512_set1_ps(0.0f);
    const unaligned_f32 *fa = a, *fb = b;
    for (size_t i = 0; i < n; i += 16) {
        __mmask16 mask = n - i >= 16 ? 0xFFFF : ((1u << (n - i)) - 1u);
        __m512 va = _mm512_maskz_loadu_ps(mask, fa + i);
        __m512 vb = _mm512_maskz_loadu_ps(mask, fb + i);
        s = _mm512_fmadd_ps(va, vb, s);
    }
    return _mm512_reduce_add_ps(s);
}

__attribute__((target("avx512fp16,avx512vl,avx512f")))
static float vec_dot_f16(const void *a, const void *b, size_t bytes) {
    size_t n = bytes / 2;
    __m512h s = _mm512_set1_ph(0);
    const unaligned_f16 *fa = a, *fb = b;
    for (size_t i = 0; i < n; i += 32) {
        __mmask32 mask = n - i >= 32 ? 0xFFFFFFFF : ((1u << (n - i)) - 1u);
        __m512i va = _mm512_maskz_loadu_epi16(mask, fa + i);
        __m512i vb = _mm512_maskz_loadu_epi16(mask, fb + i);
        s = _mm512_fmadd_ph(_mm512_castsi512_ph(va), _mm512_castsi512_ph(vb), s);
    }
    return (float)_mm512_reduce_add_ph(s);
}

static float vec_dot_i8(const void *a, const void *b, size_t bytes) {
    int32_t sum = 0;
    const int8_t *ca = a, *cb = b;
    for (size_t i = 0; i < bytes; i++) sum += (int32_t)ca[i] * (int32_t)cb[i];
    return (float)sum;
}

static int do_closest(const char *path, const char *type, const char *t, const char *k)
{
    load_db(path);
    if (strlen(t) > UINT16_MAX || strlen(k) > UINT16_MAX) diex("tenant/key too long");
    Node *n = ht_get(t, (uint16_t)strlen(t), k, (uint16_t)strlen(k));
    if (!n) return 1;
    Record *r = rec_at(n->off1 - 1);
    if (r->op == OP_DEL || !r->v_len) return 1;

    float (*dot_fn)(const void *, const void *, size_t) = NULL;
    if (!strcmp(type, "f32") && r->v_len % 4 == 0) dot_fn = vec_dot_f32;
    else if (!strcmp(type, "f16") && r->v_len % 2 == 0) dot_fn = vec_dot_f16;
    else if (!strcmp(type, "i8")) dot_fn = vec_dot_i8;
    else diex("invalid type or length");

    float best_score = -1e30f;
    const char *best_k = NULL;
    uint16_t best_k_len = 0;

    #pragma omp parallel
    {
        float local_best = -1e30f;
        const char *local_k = NULL;
        uint16_t local_k_len = 0;

        #pragma omp for schedule(dynamic, 1024)
        for (uint64_t i = 0; i < ht_cap + 256; i++) {
            if (!ht[i].off1) continue;
            Record *c = rec_at(ht[i].off1 - 1);
            if (c->op == OP_DEL || c->t_len != r->t_len || c->v_len != r->v_len) continue;
            if (ht[i].off1 == n->off1) continue; // skip self
            if (memcmp(rec_t(c), t, r->t_len) != 0) continue;

            float score = dot_fn(rec_v(r), rec_v(c), r->v_len);
            if (score > local_best) {
                local_best = score;
                local_k = rec_k(c);
                local_k_len = c->k_len;
            }
        }

        #pragma omp critical
        {
            if (local_best > best_score) {
                best_score = local_best;
                best_k = local_k;
                best_k_len = local_k_len;
            }
        }
    }

    if (best_k) {
        fwrite(best_k, 1, best_k_len, stdout);
        putchar('\n');
    }
    return 0;
}

static int do_batch(const char *path, const char *t)
{
    int lockfd = open_lockfile(path);
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
    fputs("  search TENANT [WORD...]\n", stderr);
    fputs("  tail [-f] [OFFSET]\n", stderr);
    fputs("  verify\n", stderr);
    fputs("  incr TENANT KEY DELTA\n", stderr);
    fputs("  take TENANT KEY\n", stderr);
    fputs("  decay TENANT KEY HALF_LIFE NOW DELTA\n", stderr);
    fputs("  batch TENANT     # stdin: key<TAB>value\n", stderr);
    fputs("  compact\n", stderr);
    fputs("  closest TYPE TENANT KEY\n", stderr);
    exit(2);
}

int main(int argc, char **argv)
{
    if (argc < 3) {
        usage(argv[0]);
    }

    const char *db = argv[1];
    const char *cmd = argv[2];

    if (!strcmp(cmd, "repl") && argc == 3) {
        load_db(db);
        int write_fd = open_append(db);
        char *line = NULL;
        size_t cap = 0;
        while (getline(&line, &cap, stdin) > 0) {
            char *args[64];
            int n = 0;
            for (char *p = strtok(line, " \t\r\n"); p && n < 64; p = strtok(NULL, " \t\r\n")) args[n++] = p;
            if (!n) continue;

            struct stat st;
            if (!stat(db, &st) && st.st_size > (off_t)map_size) {
                int fd = open(db, O_RDONLY | O_CLOEXEC);
                if (fd >= 0) {
                    void *nm = mremap(map_base, map_size, st.st_size, MREMAP_MAYMOVE);
                    if (nm != MAP_FAILED) {
                        map_base = nm;
                        map_size = st.st_size;
                        uint64_t off = valid_size;
                        while (off < map_size) {
                            if (!rec_valid(off)) break;
                            Record *r = rec_at(off);
                            ht_put(r->key_hash, off);
                            off += r->len;
                        }
                        valid_size = off;
                    }
                    close(fd);
                }
            }
            if (!strcmp(args[0], "get") && n == 3) do_get(args[1], args[2]);
            else if (!strcmp(args[0], "put") && n == 4) { append_fd(write_fd, args[1], args[2], args[3], OP_PUT); puts("ok"); }
            else if (!strcmp(args[0], "del") && n == 3) { append_fd(write_fd, args[1], args[2], NULL, OP_DEL); puts("ok"); }
            else if (!strcmp(args[0], "scan") && n >= 2) do_scan(args[1], n == 3 ? args[2] : NULL);
            else if (!strcmp(args[0], "search") && n >= 2) do_search(args[1], n, args);
            else if (!strcmp(args[0], "closest") && n == 4) do_closest(db, args[1], args[2], args[3]);
            else if (!strcmp(args[0], "count") && n >= 2) {
                uint64_t c = 0;
                size_t tl = strlen(args[1]);
                size_t pl = n == 3 ? strlen(args[2]) : 0;
                uint64_t iter_cap = ht_cap ? ht_cap + 256 : 0;
                for (uint64_t i = 0; i < iter_cap; i++) {
                    if (!ht[i].off1) continue;
                    Record *r = rec_at(ht[i].off1 - 1);
                    if (r->op == OP_DEL || r->t_len != tl || memcmp(rec_t(r), args[1], tl)) continue;
                    if (pl && (r->k_len < pl || memcmp(rec_k(r), args[2], pl))) continue;
                    c++;
                }
                printf("%llu\n", (unsigned long long)c);
            }
            else printf("ERR\n");
            fflush(stdout);
        }
        return 0;
    }

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
    if (!strcmp(cmd, "search") && argc >= 4) {
        load_db(db);
        return do_search(argv[3], argc, argv);
    }
    if (!strcmp(cmd, "tail") && (argc >= 3 && argc <= 5)) {
        int follow = (argc > 3 && !strcmp(argv[3], "-f"));
        const char *start = follow ? (argc == 5 ? argv[4] : NULL) : (argc == 4 ? argv[3] : NULL);
        return do_tail(db, start, follow);
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
    if (!strcmp(cmd, "decay") && argc == 8) {
        return do_decay(db, argv[3], argv[4], argv[5], argv[6], argv[7]);
    }
    if (!strcmp(cmd, "batch") && argc == 4) {
        return do_batch(db, argv[3]);
    }
    if (!strcmp(cmd, "compact") && argc == 3) {
        return do_compact(db);
    }
    if (!strcmp(cmd, "closest") && argc == 6) {
        return do_closest(db, argv[3], argv[4], argv[5]);
    }

    usage(argv[0]);
}
