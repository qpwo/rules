//bin/sh -c 'o=${0%.c}; [ "$o" -nt "$0" ] || { if [ "$(uname -s)" = Darwin ]; then p="$(brew --prefix libomp)"; ${CC:-clang} -O3 -march=native -DNDEBUG -Xpreprocessor -fopenmp -I"$p/include" -L"$p/lib" -Wl,-rpath,"$p/lib" "$0" -lomp -o "$o"; else ${CC:-gcc} -O3 -march=native -DNDEBUG -fopenmp "$0" -o "$o"; fi; } || exit; exec "$o" "$@"' "$0" "$@"; exit
/** lastdb: one-file durable append-only tenant key/value store, no sqlite, no deps. */
#define _GNU_SOURCE
#include <errno.h>
#include <fcntl.h>
#include <sys/file.h>
#include <limits.h>
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
#include <omp.h>
#include <sched.h>



#define MAGIC 0x3144534cU
#define OP_PUT 1
#define OP_DEL 2
#define FNV0 0xcbf29ce484222325ULL
#define FNV1 0x100000001b3ULL

typedef struct __attribute__((packed)) {
    uint32_t magic;
    uint32_t len;
    uint8_t t_len;
    uint8_t weight_log;
    uint16_t k_len;
    uint32_t v_len;
    uint8_t op;
    uint64_t bf;
    uint64_t key_hash;
    uint64_t check;
} Record;

static uint64_t compute_bf(const char *data, size_t len) {
    uint64_t bf = 0;
    const unsigned char *p = (const unsigned char *)data;
    for (size_t i = 0; i + 2 < len; i++) {
        uint32_t gram = p[i] | (p[i+1] << 8) | (p[i+2] << 16);
        gram *= 0x85ebca6b;
        gram ^= gram >> 13;
        gram *= 0xc2b2ae35;
        gram ^= gram >> 16;
        bf |= 1ULL << (gram & 63);
    }
    return bf;
}

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
static uint64_t ht_sorted_len;

static void deduplicate_ht(void);

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
    uint64_t th = fnv_bytes(FNV0, t, tl);
    uint64_t kh = fnv_bytes(FNV0, k, kl);
    uint64_t h = (th << 32) | (kh >> 32);
    return h ? h : 1;
}

static uint64_t ht_lower_bound(uint64_t hash) {
    uint64_t lo = 0;
    uint64_t hi = ht_sorted_len;
    while (lo < hi) {
        uint64_t mid = lo + (hi - lo) / 2;
        if (ht[mid].hash < hash) {
            lo = mid + 1;
        } else {
            hi = mid;
        }
    }
    return lo;
}

static void ht_tenant_range(const char *t, uint16_t tl, uint64_t *start_idx, uint64_t *end_idx) {
    if (ht_sorted_len != ht_len) {
        deduplicate_ht();
    }
    if (!ht_sorted_len) { *start_idx = 0; *end_idx = 0; return; }
    uint64_t th = fnv_bytes(FNV0, t, tl);
    uint64_t h_start = th << 32;
    uint64_t h_end = h_start | 0xFFFFFFFFULL;
    if (h_start == 0) h_start = 1;
    *start_idx = ht_lower_bound(h_start);
    *end_idx = h_end == UINT64_MAX ? ht_sorted_len : ht_lower_bound(h_end + 1);
}

static uint64_t rec_check(const Record *r, const char *t, const char *k, const char *v)
{
    uint64_t h = FNV0;
    h = fnv_u64(h, r->magic);
    h = fnv_u64(h, r->len);
    h = fnv_u64(h, r->t_len);
    h = fnv_u64(h, r->weight_log);
    h = fnv_u64(h, r->k_len);
    h = fnv_u64(h, r->v_len);
    h = fnv_u64(h, r->op);
    h = fnv_u64(h, r->bf);
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



static void reserve_ram(size_t bytes)
{
    struct sysinfo si;
    if (sysinfo(&si)) die("sysinfo");

    uint64_t total = (uint64_t)si.totalram * si.mem_unit;
    uint64_t freeish = (uint64_t)(si.freeram + si.bufferram) * si.mem_unit;
    if (freeish < total / 10 + bytes) {
        diex("ram reserve below 10 percent");
    }
}

static int worker_threads(void)
{
    int n = omp_get_num_procs();
    int keep = (n + 9) / 10;
    return n > keep ? n - keep : 1;
}

static void ht_reserve(uint64_t need)
{
    if (need <= ht_cap) {
        return;
    }

    uint64_t ncap = ht_cap ? ht_cap : 4096;
    while (ncap < need) {
        if (ncap > UINT64_MAX / 2) {
            diex("ht too large");
        }
        ncap *= 2;
    }

    size_t old_bytes = ht_cap * sizeof(*ht);
    size_t bytes = ncap * sizeof(*ht);
    if (bytes / sizeof(*ht) != ncap) {
        diex("ht too large");
    }

    reserve_ram(bytes - old_bytes);
    if (ht) {
        Node *nht = mremap(ht, old_bytes, bytes, MREMAP_MAYMOVE);
        if (nht == MAP_FAILED) {
            die("mremap ht");
        }
        ht = nht;
    } else {
        ht = mmap(NULL, bytes, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
        if (ht == MAP_FAILED) {
            die("mmap ht");
        }
    }
    madvise(ht, bytes, MADV_HUGEPAGE);
    ht_cap = ncap;
}

static void ht_put(uint64_t hash, uint64_t off)
{
    if ((ht_len >= ht_cap && ht_len >= 1048576) || ht_len - ht_sorted_len >= 65536) {
        deduplicate_ht();
    }
    ht_reserve(ht_len + 1);
    int stays_sorted = ht_len == ht_sorted_len && (!ht_len || ht[ht_len - 1].hash <= hash);
    ht[ht_len++] = (Node){hash, off + 1};
    if (stays_sorted) {
        ht_sorted_len = ht_len;
    }
}

static int cmp_node(const void *a, const void *b) {
    const Node *x = a, *y = b;
    if (x->hash != y->hash) return x->hash < y->hash ? -1 : 1;
    return x->off1 < y->off1 ? -1 : 1;
}

static void deduplicate_ht(void) {
    if (!ht_len) {
        ht_sorted_len = 0;
        return;
    }
    qsort(ht, ht_len, sizeof(*ht), cmp_node);
    uint64_t out = 0;
    for (uint64_t i = 0; i < ht_len; ) {
        uint64_t j = i + 1;
        while (j < ht_len && ht[j].hash == ht[i].hash) j++;
        for (uint64_t k = j; k-- > i; ) {
            Record *rk = rec_at(ht[k].off1 - 1);
            int dup = 0;
            for (uint64_t prev = out; prev > 0 && ht[prev-1].hash == ht[i].hash; prev--) {
                Record *rp = rec_at(ht[prev-1].off1 - 1);
                if (rk->t_len == rp->t_len && rk->k_len == rp->k_len &&
                    !memcmp(rec_t(rk), rec_t(rp), rk->t_len) &&
                    !memcmp(rec_k(rk), rec_k(rp), rk->k_len)) {
                    dup = 1; break;
                }
            }
            if (!dup) ht[out++] = ht[k];
        }
        i = j;
    }
    ht_len = out;
    ht_sorted_len = out;
}


static Node *ht_get(const char *t, uint16_t tl, const char *k, uint16_t kl)
{
    if (!ht_len) return NULL;
    uint64_t hash = key_hash(t, tl, k, kl);
    for (uint64_t i = ht_len; i-- > ht_sorted_len; ) {
        if (ht[i].hash == hash && key_eq(ht[i].off1 - 1, t, tl, k, kl)) return ht + i;
    }
    uint64_t idx = ht_lower_bound(hash);
    Node *begin = ht + idx;
    Node *end = ht + ht_sorted_len;
    while (begin < end && begin->hash == hash) {
        if (key_eq(begin->off1 - 1, t, tl, k, kl)) return begin;
        begin++;
    }
    return NULL;
}

static void load_db(const char *path)
{
    struct stat st;
    if (stat(path, &st) || st.st_size <= (off_t)map_size) {
        return;
    }
    int fd = open(path, O_RDONLY | O_CLOEXEC);
    if (fd < 0) {
        die("open");
    }
    void *nm = map_size ? mremap(map_base, map_size, st.st_size, MREMAP_MAYMOVE) : mmap(NULL, st.st_size, PROT_READ, MAP_SHARED, fd, 0);
    if (nm == MAP_FAILED) {
        die("mmap/mremap");
    }
    map_base = nm;
    map_size = st.st_size;
    if (close(fd)) {
        die("close");
    }
    (void)posix_madvise(map_base, map_size, POSIX_MADV_SEQUENTIAL);
    (void)posix_madvise(map_base, map_size, POSIX_MADV_NOREUSE);
    uint64_t off = valid_size;
    uint64_t start_len = ht_len;
    uint64_t added = 0;
    while (off < map_size) {
        if (!rec_valid(off)) {
            break;
        }
        Record *r = rec_at(off);
        ht_put(r->key_hash, off);
        off += r->len;
        added++;
    }
    valid_size = off;
    if (added && (!start_len || added >= 1024)) {
        deduplicate_ht();
    }
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

static int append_raw(int fd, const char *t, size_t tl, const char *k, size_t kl, const char *v, size_t vl, uint8_t op)
{
    if (!v || op == OP_DEL) {
        v = "";
        vl = 0;
    }
    if (tl > 255 || kl > UINT16_MAX) diex("tenant/key too long");
    if (vl > UINT32_MAX) diex("value too long");
    if (sizeof(Record) + tl + kl + vl > UINT32_MAX) diex("record too long");

    Record r = {0};
    r.magic = MAGIC;
    r.len = (uint32_t)(sizeof(r) + tl + kl + vl);
    r.t_len = (uint8_t)tl;
    r.weight_log = 0;
    r.k_len = (uint16_t)kl;
    r.v_len = (uint32_t)vl;
    r.op = op;

    struct statvfs st;
    if (op != OP_DEL && !fstatvfs(fd, &st) && st.f_blocks > 0 && st.f_frsize > 0) {
        uint64_t free_bytes = (uint64_t)st.f_bavail * st.f_frsize;
        uint64_t reserve_bytes = (uint64_t)((long double)st.f_blocks * st.f_frsize * 0.1L);
        double avail = (double)st.f_bavail / st.f_blocks;
        double keep = __builtin_exp2(50.0 * (avail - 0.2));
        double gate = (double)(key_hash(t, r.t_len, k, r.k_len) >> 32) * 0x1.0p-32;
        if (free_bytes < reserve_bytes + r.len) {
            return 0;
        }
        if (avail < 0.2) {
            if (gate > keep) return 0;
            int wl = (int)(-50.0 * (avail - 0.2));
            r.weight_log = wl > 31 ? 31 : wl;
        }
    }

    r.bf = compute_bf(k, kl) | compute_bf(v, vl);
    r.key_hash = key_hash(t, r.t_len, k, r.k_len);
    r.check = rec_check(&r, t, k, v);

    off_t pos = lseek(fd, 0, SEEK_END);
    if (pos >= 0) {
        off_t end = pos + (off_t)r.len;
        if ((pos & 33554431) == 0 || ((pos ^ end) >> 25)) {
            (void)fallocate(fd, FALLOC_FL_KEEP_SIZE, 0, (end + 33554431) & ~(off_t)33554431);
        }
    }

    struct iovec iov[4] = {
        {&r, sizeof(r)},
        {(void *)t, tl},
        {(void *)k, kl},
        {(void *)v, vl},
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
    return 1;
}

static int append_fd(int fd, const char *t, const char *k, const char *v, uint8_t op)
{
    if (!append_raw(fd, t, strlen(t), k, strlen(k), v ? v : "", v ? strlen(v) : 0, op)) {
        diex("shed");
    }
    return 1;
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
    if (!*s || errno || *end || !__builtin_isfinite(x)) {
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
    int had = current_decay(t, k, &stored_hl, &last, &value);
    if (had) {
        if (stored_hl != hl) {
            close(lockfd);
            diex("decay half life changed");
        }
        if (ts < last) {
            close(lockfd);
            diex("time went backwards");
        }
    }

    double next = value * __builtin_exp2((last - ts) / hl) + d;
    if (!__builtin_isfinite(next)) {
        close(lockfd);
        diex("decay value not finite");
    }
    if (__builtin_fabs(next) < 1e-12) {
        if (had) {
            int fd = open_append(path);
            append_fd(fd, t, k, NULL, OP_DEL);
            sync_fd(fd);
            if (close(fd)) {
                die("close");
            }
        }
        close(lockfd);
        puts("0");
        return 0;
    }

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

    for (uint64_t i = 0; i < ht_len; i++) {
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
            printf("%llu\t%llu\t%s\t%u\t", (unsigned long long)next, (unsigned long long)off, r->op == OP_PUT ? "put" : "del", 1U << r->weight_log);
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

static void term_lens(int argc, char **argv, size_t *lens)
{
    for (int i = 4; i < argc; i++) {
        lens[i] = strlen(argv[i]);
    }
    for (int i = 4; i + 1 < argc; i++) {
        for (int j = i + 1; j < argc; j++) {
            if (lens[j] <= lens[i]) {
                continue;
            }
            size_t nl = lens[i];
            lens[i] = lens[j];
            lens[j] = nl;
            char *na = argv[i];
            argv[i] = argv[j];
            argv[j] = na;
        }
    }
}

static unsigned byte_rank(unsigned char c)
{
    if (c == '\t' || c == '\n' || c == 0) return 1;
    if (c >= 'A' && c <= 'Z') return 2;
    if (c >= '0' && c <= '9') return 3;
    if (c <= ' ') return 32;
    if (c >= 'a' && c <= 'z') return 16;
    if (c == '_' || c == '-' || c == '/' || c == '.') return 8;
    return 4;
}

static const void *memmem_pivot(const void *haystack, size_t hay_len, const void *needle, size_t needle_len)
{
    const unsigned char *hay = haystack;
    const unsigned char *need = needle;
    size_t pivot = 0;
    unsigned best = UINT_MAX;

    if (!needle_len) return hay;
    if (needle_len > hay_len) return NULL;
    if (needle_len == 1) return memchr(hay, need[0], hay_len);

    for (size_t i = 0; i < needle_len; i++) {
        unsigned rank = byte_rank(need[i]);
        if (rank < best) {
            best = rank;
            pivot = i;
        }
    }

    const unsigned char *p = hay + pivot;
    const unsigned char *end = hay + hay_len - (needle_len - pivot) + 1;
    while (p < end) {
        p = memchr(p, need[pivot], end - p);
        if (!p) return NULL;
        if (!memcmp(p - pivot, need, needle_len)) return p - pivot;
        p++;
    }
    return NULL;
}

static int rec_has_terms(Record *r, int argc, char **argv, size_t *lens, uint64_t *term_bfs)
{
    for (int j = 4; j < argc; j++) {
        if ((r->bf & term_bfs[j]) != term_bfs[j]) {
            return 0;
        }
        if (!memmem_pivot(rec_v(r), r->v_len, argv[j], lens[j]) &&
            !memmem_pivot(rec_k(r), r->k_len, argv[j], lens[j])) {
            return 0;
        }
    }
    return 1;
}

static void write_weighted_record(Record *r);

static int do_search(const char *t, int argc, char **argv)
{
    size_t tl = strlen(t);
    if (tl > UINT16_MAX || !ht_cap) {
        return 0;
    }

    size_t lens[argc];
    term_lens(argc, argv, lens);
    for (int i = 4; i < argc; i++) {
        if (lens[i] < 3) {
            diex("search terms need at least 3 bytes");
        }
    }

    uint64_t term_bfs[argc];
    for (int i = 4; i < argc; i++) term_bfs[i] = compute_bf(argv[i], lens[i]);

    uint64_t start_idx, end_idx;
    ht_tenant_range(t, tl, &start_idx, &end_idx);
    #pragma omp parallel for schedule(dynamic, 1024) num_threads(worker_threads())
    for (uint64_t i = start_idx; i < end_idx; i++) {
        Record *r = rec_at(ht[i].off1 - 1);
        if (r->op == OP_DEL || r->t_len != tl) {
            continue;
        }
        if (memcmp(rec_t(r), t, tl)) {
            continue;
        }
        if (!rec_has_terms(r, argc, argv, lens, term_bfs)) {
            continue;
        }

        #pragma omp critical
        {
            write_weighted_record(r);
        }
    }
    return 0;
}

static void write_weighted_record(Record *r)
{
    if (printf("%u\t", 1U << r->weight_log) < 0) {
        die("printf");
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

    uint64_t start_idx, end_idx;
    ht_tenant_range(t, tl, &start_idx, &end_idx);
    for (uint64_t i = start_idx; i < end_idx; i++) {
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
        write_weighted_record(r);
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
    enum { COMPACT_WRITE_BYTES = 32 * 1024 * 1024 };

    int lockfd = open_lockfile(path);
    load_db(path);

    uint64_t live_bytes = 0;
    for (uint64_t i = 0; i < ht_len; i++) {
        Record *r = rec_at(ht[i].off1 - 1);
        if (r->op == OP_DEL) {
            continue;
        }
        if (UINT64_MAX - live_bytes < r->len) {
            diex("compact live bytes overflow");
        }
        live_bytes += r->len;
    }

    char tmp[4096];
    snprintf(tmp, sizeof(tmp), "%s.tmp", path);

    int fd = open(tmp, O_WRONLY | O_CREAT | O_TRUNC | O_CLOEXEC, 0666);
    if (fd < 0) {
        die("open tmp");
    }

    struct statvfs st;
    if (!fstatvfs(fd, &st) && st.f_blocks > 0 && st.f_frsize > 0) {
        uint64_t free_bytes = (uint64_t)st.f_bavail * st.f_frsize;
        uint64_t reserve_bytes = (uint64_t)((long double)st.f_blocks * st.f_frsize * 0.1L);
        if (free_bytes < reserve_bytes || free_bytes - reserve_bytes < live_bytes) {
            diex("compact needs temp space plus 10 percent disk reserve");
        }
    }

    (void)posix_fadvise(fd, 0, 0, POSIX_FADV_SEQUENTIAL);
    reserve_ram(COMPACT_WRITE_BYTES);
    char *buf = mmap(NULL, COMPACT_WRITE_BYTES, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS | MAP_POPULATE, -1, 0);
    if (buf == MAP_FAILED) {
        die("mmap compact");
    }
    madvise(buf, COMPACT_WRITE_BYTES, MADV_HUGEPAGE);

    size_t used = 0;
    for (uint64_t off = 0; off < valid_size;) {
        Record *r = rec_at(off);
        Node *n = ht_get(rec_t(r), r->t_len, rec_k(r), r->k_len);
        if (n && (n->off1 - 1) == off && r->op != OP_DEL) {
            if (r->len > COMPACT_WRITE_BYTES) {
                if (used) {
                    write_all(fd, buf, used);
                    used = 0;
                }
                write_all(fd, r, r->len);
            } else {
                if (COMPACT_WRITE_BYTES - used < r->len) {
                    write_all(fd, buf, used);
                    used = 0;
                }
                memcpy(buf + used, r, r->len);
                used += r->len;
            }
        }
        off += r->len;
    }
    if (used) {
        write_all(fd, buf, used);
    }

    if (munmap(buf, COMPACT_WRITE_BYTES)) {
        die("munmap compact");
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
typedef uint16_t unaligned_f16 __attribute__((aligned(1)));

__attribute__((target("avx512f,avx512vl,fma")))
static float vec_dot_f32(const void *a, const void *b, size_t bytes) {
    size_t n = bytes / 4;
    const unaligned_f32 *fa = a, *fb = b;
    __m512 sum = _mm512_setzero_ps();

    for (size_t i = 0; i < n; i += 16) {
        __mmask16 mask = n - i >= 16 ? 0xFFFF : (__mmask16)((1u << (n - i)) - 1u);
        sum = _mm512_fmadd_ps(_mm512_maskz_loadu_ps(mask, fa + i), _mm512_maskz_loadu_ps(mask, fb + i), sum);
    }

    return _mm512_reduce_add_ps(sum);
}

__attribute__((target("avx512f,avx512bw,avx512vl,f16c,fma")))
static float vec_dot_f16(const void *a, const void *b, size_t bytes) {
    size_t n = bytes / 2;
    const unaligned_f16 *fa = a, *fb = b;
    __m512 sum = _mm512_setzero_ps();

    for (size_t i = 0; i < n; i += 16) {
        __mmask16 mask = n - i >= 16 ? 0xFFFF : (__mmask16)((1u << (n - i)) - 1u);
        sum = _mm512_fmadd_ps(_mm512_cvtph_ps(_mm256_maskz_loadu_epi16(mask, fa + i)), _mm512_cvtph_ps(_mm256_maskz_loadu_epi16(mask, fb + i)), sum);
    }

    return _mm512_reduce_add_ps(sum);
}

__attribute__((target("avx2")))
static float vec_dot_i8(const void *a, const void *b, size_t bytes) {
    size_t n = bytes;
    __m256i sum0 = _mm256_setzero_si256(), sum1 = _mm256_setzero_si256();
    const int8_t *ca = a, *cb = b;
    size_t i = 0;
    for (; i + 63 < n; i += 64) {
        __m256i va0 = _mm256_loadu_si256((const __m256i*)(ca + i));
        __m256i vb0 = _mm256_loadu_si256((const __m256i*)(cb + i));
        __m256i va1 = _mm256_loadu_si256((const __m256i*)(ca + i + 32));
        __m256i vb1 = _mm256_loadu_si256((const __m256i*)(cb + i + 32));
        sum0 = _mm256_add_epi32(sum0, _mm256_madd_epi16(_mm256_cvtepi8_epi16(_mm256_castsi256_si128(va0)), _mm256_cvtepi8_epi16(_mm256_castsi256_si128(vb0))));
        sum0 = _mm256_add_epi32(sum0, _mm256_madd_epi16(_mm256_cvtepi8_epi16(_mm256_extracti128_si256(va0, 1)), _mm256_cvtepi8_epi16(_mm256_extracti128_si256(vb0, 1))));
        sum1 = _mm256_add_epi32(sum1, _mm256_madd_epi16(_mm256_cvtepi8_epi16(_mm256_castsi256_si128(va1)), _mm256_cvtepi8_epi16(_mm256_castsi256_si128(vb1))));
        sum1 = _mm256_add_epi32(sum1, _mm256_madd_epi16(_mm256_cvtepi8_epi16(_mm256_extracti128_si256(va1, 1)), _mm256_cvtepi8_epi16(_mm256_extracti128_si256(vb1, 1))));
    }
    sum0 = _mm256_add_epi32(sum0, sum1);
    for (; i + 31 < n; i += 32) {
        __m256i va = _mm256_loadu_si256((const __m256i*)(ca + i));
        __m256i vb = _mm256_loadu_si256((const __m256i*)(cb + i));
        sum0 = _mm256_add_epi32(sum0, _mm256_madd_epi16(_mm256_cvtepi8_epi16(_mm256_castsi256_si128(va)), _mm256_cvtepi8_epi16(_mm256_castsi256_si128(vb))));
        sum0 = _mm256_add_epi32(sum0, _mm256_madd_epi16(_mm256_cvtepi8_epi16(_mm256_extracti128_si256(va, 1)), _mm256_cvtepi8_epi16(_mm256_extracti128_si256(vb, 1))));
    }
    int32_t buf[8];
    _mm256_storeu_si256((__m256i*)buf, sum0);
    int32_t sum = buf[0] + buf[1] + buf[2] + buf[3] + buf[4] + buf[5] + buf[6] + buf[7];
    for (; i < n; i++) sum += (int32_t)ca[i] * (int32_t)cb[i];
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

    uint64_t start_idx, end_idx;
    ht_tenant_range(t, r->t_len, &start_idx, &end_idx);

    #pragma omp parallel num_threads(worker_threads())
    {
        float local_best = -1e30f;
        const char *local_k = NULL;
        uint16_t local_k_len = 0;

        #pragma omp for schedule(dynamic, 1024)
        for (uint64_t i = start_idx; i < end_idx; i++) {
            Record *c = rec_at(ht[i].off1 - 1);
            if (c->op == OP_DEL || c->t_len != r->t_len || c->v_len != r->v_len) continue;
            if (ht[i].off1 == n->off1) continue; // skip self
            if (memcmp(rec_t(c), t, r->t_len) != 0) continue;

            size_t p_len = r->v_len > 256 ? 256 : r->v_len;
            float p_score = dot_fn(rec_v(r), rec_v(c), p_len);
            if (p_len < r->v_len && p_score * ((float)r->v_len / p_len) < local_best - 0.5f) continue;
            float score = p_len < r->v_len ? p_score + dot_fn(rec_v(r) + p_len, rec_v(c) + p_len, r->v_len - p_len) : p_score;
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

enum { BATCH_WRITE_BYTES = 32 * 1024 * 1024 };

typedef struct {
    uint8_t data[64 * 1024 * 1024];
} Chunk;

static inline void chunk_push(Chunk *chunk);
static inline Chunk *chunk_pop(void);

static int batch_flush(int fd, char *buf, size_t *used)
{
    if (!*used) {
        return 1;
    }

    struct statvfs st;
    if (!fstatvfs(fd, &st) && st.f_blocks > 0 && st.f_frsize > 0) {
        uint64_t free_bytes = (uint64_t)st.f_bavail * st.f_frsize;
        uint64_t reserve_bytes = (uint64_t)((long double)st.f_blocks * st.f_frsize * 0.1L);
        if (free_bytes < reserve_bytes + *used) {
            return 0;
        }
    }

    write_all(fd, buf, *used);
    *used = 0;
    return 1;
}

static int batch_pressure(int fd, size_t need, double *keep, uint8_t *weight_log)
{
    *keep = 1.0;
    *weight_log = 0;

    struct statvfs st;
    if (fstatvfs(fd, &st) || st.f_blocks == 0 || st.f_frsize == 0) {
        return 1;
    }

    uint64_t free_bytes = (uint64_t)st.f_bavail * st.f_frsize;
    uint64_t reserve_bytes = (uint64_t)((long double)st.f_blocks * st.f_frsize * 0.1L);
    if (free_bytes < reserve_bytes + need) {
        return 0;
    }

    double avail = (double)st.f_bavail / st.f_blocks;
    if (avail >= 0.2) {
        return 1;
    }

    *keep = __builtin_exp2(50.0 * (avail - 0.2));
    int wl = (int)(-50.0 * (avail - 0.2));
    *weight_log = wl > 31 ? 31 : wl;
    return 1;
}

static int batch_put(int fd, char *buf, size_t *used, const char *t, const char *k, const char *v)
{
    static double keep = 1.0;
    static uint8_t weight_log = 0;

    size_t tl = strlen(t);
    size_t kl = strlen(k);
    size_t vl = strlen(v);
    if (tl > 255 || kl > UINT16_MAX) diex("tenant/key too long");
    if (vl > UINT32_MAX) diex("value too long");
    if (sizeof(Record) + tl + kl + vl > UINT32_MAX) diex("record too long");

    Record r = {0};
    r.magic = MAGIC;
    r.len = (uint32_t)(sizeof(r) + tl + kl + vl);
    r.t_len = (uint8_t)tl;
    r.k_len = (uint16_t)kl;
    r.v_len = (uint32_t)vl;
    r.op = OP_PUT;

    if (r.len > BATCH_WRITE_BYTES) {
        if (!batch_flush(fd, buf, used)) return 0;
        return append_raw(fd, t, tl, k, kl, v, vl, OP_PUT);
    }
    if (BATCH_WRITE_BYTES - *used < r.len && !batch_flush(fd, buf, used)) {
        return 0;
    }
    if (!*used && !batch_pressure(fd, r.len, &keep, &weight_log)) {
        return 0;
    }
    if (keep < 1.0) {
        double gate = (double)(key_hash(t, r.t_len, k, r.k_len) >> 32) * 0x1.0p-32;
        if (gate > keep) {
            return 1;
        }
        r.weight_log = weight_log;
    }

    r.bf = compute_bf(k, kl) | compute_bf(v, vl);
    r.key_hash = key_hash(t, r.t_len, k, r.k_len);
    r.check = rec_check(&r, t, k, v);

    memcpy(buf + *used, &r, sizeof(r));
    *used += sizeof(r);
    memcpy(buf + *used, t, tl);
    *used += tl;
    memcpy(buf + *used, k, kl);
    *used += kl;
    memcpy(buf + *used, v, vl);
    *used += vl;
    return 1;
}

static int do_batch(const char *path, const char *t)
{
    int lockfd = open_lockfile(path);
    load_db(path);
    int fd = open_append(path);
    char *line = NULL;
    char *buf = NULL;
    size_t cap = 0;
    size_t used = 0;
    int ok = 1;

    Chunk *batch_c = chunk_pop();
    if (!batch_c) diex("out of resources");
    buf = (char *)batch_c->data;

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
        if (!batch_put(fd, buf, &used, t, line, tab + 1)) {
            ok = 0;
            break;
        }
    }

    if (ferror(stdin)) {
        die("getline");
    }
    if (!batch_flush(fd, buf, &used)) {
        ok = 0;
    }
    chunk_push(batch_c);
    free(line);
    sync_fd(fd);
    if (close(fd)) {
        die("close");
    }
    close(lockfd);
    if (!ok) {
        diex("shed");
    }
    return 0;
}

#include <arpa/inet.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <sys/socket.h>

static void crypt_buf(uint8_t *buf, size_t len, int32_t key) {
    if (!key) return;
    uint32_t k = (uint32_t)key;
    for (size_t i = 0; i < len; i++) {
        k ^= k << 13; k ^= k >> 17; k ^= k << 5;
        buf[i] ^= (k & 255);
    }
}

static int read_full(int fd, void *buf, size_t n) {
    char *p = buf;
    while (n > 0) {
        ssize_t r = read(fd, p, n);
        if (r < 0 && errno == EINTR) continue;
        if (r <= 0) return 0;
        p += r;
        n -= r;
    }
    return 1;
}

static int write_full(int fd, const void *buf, size_t n) {
    const char *p = buf;
    while (n > 0) {
        ssize_t r = write(fd, p, n);
        if (r < 0 && errno == EINTR) continue;
        if (r <= 0) return 0;
        p += r;
        n -= r;
    }
    return 1;
}

static void send_response(int fd, int32_t cipherkey, int32_t status, const void *payload, uint32_t n) {
    if (n > 64u * 1024u * 1024u - 16u) {
        status = 3;
        payload = "too large";
        n = 9;
    }

    uint32_t len = 16 + n;
    if (!cipherkey) {
        uint8_t head[20];
        *(uint32_t*)(head + 0) = htonl(len);
        *(uint32_t*)(head + 4) = htonl(0x4c444231);
        *(uint32_t*)(head + 8) = htonl(status);
        *(uint32_t*)(head + 12) = 0;
        *(uint32_t*)(head + 16) = htonl(n);
        write_full(fd, head, sizeof(head));
        if (n) write_full(fd, payload, n);
        return;
    }

    Chunk *resp_c = NULL;
    uint8_t small[4096];
    uint8_t *buf = small;
    if (4 + len > sizeof(small)) {
        resp_c = chunk_pop();
        if (!resp_c) return;
        buf = resp_c->data;
    }
    *(uint32_t*)(buf + 0) = htonl(len);
    *(uint32_t*)(buf + 4) = htonl(0x4c444231);
    *(uint32_t*)(buf + 8) = htonl(status);
    *(uint32_t*)(buf + 12) = 0;
    *(uint32_t*)(buf + 16) = htonl(n);
    if (n) memcpy(buf + 20, payload, n);
    crypt_buf(buf + 4, len, cipherkey);
    write_full(fd, buf, 4 + len);
    if (resp_c) chunk_push(resp_c);
}

static __thread Chunk *chunk_freelist;

static inline void chunk_push(Chunk *chunk) {
    *(Chunk **)chunk = chunk_freelist;
    chunk_freelist = chunk;
}

static inline Chunk *chunk_pop(void) {
    Chunk *chunk = chunk_freelist;
    if (chunk) {
        chunk_freelist = *(Chunk **)chunk;
        return chunk;
    }

    struct sysinfo si;
    if (!sysinfo(&si)) {
        uint64_t total = (uint64_t)si.totalram * si.mem_unit;
        uint64_t freeish = (uint64_t)(si.freeram + si.bufferram) * si.mem_unit;
        if (freeish < total / 10 + sizeof(*chunk)) return NULL;
    }
    double load[1];
    if (getloadavg(load, 1) == 1 && load[0] > omp_get_num_procs() * 0.9) return NULL;
    chunk = mmap(NULL, sizeof(*chunk), PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS | MAP_POPULATE, -1, 0);
    if (chunk == MAP_FAILED) return NULL;
    madvise(chunk, sizeof(*chunk), MADV_HUGEPAGE);
    return chunk;
}

static int srv_db_fd = -1;
#include <sys/resource.h>
static void do_serve(const char *db_path, int port, int32_t cipherkey) {
    omp_set_dynamic(0);
    omp_set_max_active_levels(2);

    struct rlimit rl;
    if (getrlimit(RLIMIT_NOFILE, &rl)) die("getrlimit");
    int max_conn = (rl.rlim_cur > 100 ? rl.rlim_cur : 100) * 9 / 10;
    _Atomic int active_conn = 0;

    int srv = socket(AF_INET, SOCK_STREAM, 0);
    if (srv < 0) die("socket");
    int opt = 1;
    setsockopt(srv, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
    struct sockaddr_in addr = {0};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = INADDR_ANY;
    if (bind(srv, (struct sockaddr *)&addr, sizeof(addr)) < 0) die("bind");
    if (listen(srv, SOMAXCONN) < 0) die("listen");
    printf("Listening on port %d...\n", port);

    #pragma omp parallel num_threads(worker_threads())
    {
        #pragma omp single
        {
            while (1) {
                int fd = accept(srv, NULL, NULL);
                if (fd >= 0) {
                    if (active_conn >= max_conn) {
                        close(fd);
                        continue;
                    }
                    active_conn++;
                    setsockopt(fd, IPPROTO_TCP, TCP_NODELAY, &opt, sizeof(opt));
                    #pragma omp task
                    {
                        while (1) {
                            uint32_t len;
                            if (!read_full(fd, &len, 4)) break;
                            len = ntohl(len);
                            if (len > 64 * 1024 * 1024) break;

                            Chunk *c = chunk_pop();
                            if (!c) break;
                            if (len > sizeof(c->data)) { chunk_push(c); break; }
                            uint8_t *buf = c->data;

                            if (!read_full(fd, buf, len)) { chunk_push(c); break; }
                            crypt_buf(buf, len, cipherkey);

                            if (len < 32) { chunk_push(c); break; }
                            uint32_t magic = ntohl(*(uint32_t*)(buf + 0));
                            if (magic != 0x4c444231) { chunk_push(c); break; }
                            int32_t op = ntohl(*(uint32_t*)(buf + 4));
                            int32_t user = ntohl(*(uint32_t*)(buf + 8));
                            int32_t pass = ntohl(*(uint32_t*)(buf + 12));
                            int32_t color = ntohl(*(uint32_t*)(buf + 16));
                            uint32_t tl = ntohl(*(uint32_t*)(buf + 20));
                            uint32_t kl = ntohl(*(uint32_t*)(buf + 24));
                            uint32_t vl = ntohl(*(uint32_t*)(buf + 28));

                            if (tl > 200 || kl > UINT16_MAX || vl > len || 32 + tl + kl + vl > len) { chunk_push(c); break; }

                            char *t = (char*)buf + 32;
                            char *k = (char*)buf + 32 + tl;
                            char *v = (char*)buf + 32 + tl + kl;

                            int perms = 0;
                            #pragma omp critical (db)
                            {
                                load_db(db_path);
                                char ustr[64];
                                int ulen = snprintf(ustr, sizeof(ustr), "%d:%d", user, color);
                                if (ulen > 0 && ulen < (int)sizeof(ustr)) {
                                    Node *n = ht_get("0:users", 7, ustr, ulen);
                                    if (n) {
                                        Record *r = rec_at(n->off1 - 1);
                                        if (r->op != OP_DEL && r->v_len >= 3) {
                                            char vbuf[64] = {0};
                                            memcpy(vbuf, rec_v(r), r->v_len < 63 ? r->v_len : 63);
                                            int32_t sp = 0, sprm = 0;
                                            if (sscanf(vbuf, "%d,%d", &sp, &sprm) == 2 && sp == pass) perms = sprm;
                                        }
                                    }
                                }
                                if (user == 0 && pass == 0) perms = 7;
                            }

                            char full_tenant[65536];
                            int ft_len = snprintf(full_tenant, sizeof(full_tenant), "%d:", color);
                            if (ft_len <= 0 || ft_len + (int)tl >= (int)sizeof(full_tenant)) { chunk_push(c); break; }
                            memcpy(full_tenant + ft_len, t, tl);
                            ft_len += tl;
                            full_tenant[ft_len] = 0;

                            if (op == 1) {
                                if (!(perms & 1)) { send_response(fd, cipherkey, 1, "denied", 6); chunk_push(c); continue; }
                                #pragma omp critical (db)
                                {
                                    load_db(db_path);
                                    Node *n = ht_get(full_tenant, ft_len, k, kl);
                                    if (n) {
                                        Record *r = rec_at(n->off1 - 1);
                                        if (r->op != OP_DEL) send_response(fd, cipherkey, 0, rec_v(r), r->v_len);
                                        else send_response(fd, cipherkey, 2, "not found", 9);
                                    } else send_response(fd, cipherkey, 2, "not found", 9);
                                }
                            }
                            else if (op == 4 || op == 5) {
                                if (!(perms & 1)) { send_response(fd, cipherkey, 1, "denied", 6); chunk_push(c); continue; }
                                Chunk *out_c = chunk_pop();
                                if (!out_c) { send_response(fd, cipherkey, 3, "shed", 4); chunk_push(c); continue; }
                                uint8_t *out = out_c->data; size_t out_len = 0;
                                uint64_t query_bfs[64] = {0};
                            if (op == 5) {
                                int num_words = 0; char *w = v; char *end = v + vl;
                                while (w < end && num_words < 64) {
                                    char *tab = memchr(w, '\t', end - w);
                                    size_t wl = tab ? tab - w : end - w;
                                    if (wl > 0) query_bfs[num_words++] = compute_bf(w, wl);
                                    w += wl + 1;
                                }
                            }
                            #define OUT_CAP (60u * 1024u * 1024u)
                            #define APP(ptr, lll) do { size_t app_n = (lll); if (out_len <= OUT_CAP && app_n <= OUT_CAP - out_len) { memcpy(out + out_len, ptr, app_n); out_len += app_n; } else { out_len = OUT_CAP + 1; } } while(0)
                            #pragma omp critical (db)
                            {
                                load_db(db_path);
                            uint64_t start_idx, end_idx;
                            ht_tenant_range(full_tenant, ft_len, &start_idx, &end_idx);
                            #pragma omp parallel for schedule(dynamic, 1024) num_threads(worker_threads())
                            for (uint64_t i = start_idx; i < end_idx; i++) {
                                Record *r = rec_at(ht[i].off1 - 1);
                                if (r->op == OP_DEL || r->t_len != ft_len || memcmp(rec_t(r), full_tenant, ft_len)) continue;
                                    int match = 0;
                                    if (op == 4) {
                                        match = (kl == 0 || (r->k_len >= kl && !memcmp(rec_k(r), k, kl)));
                                    } else {
                                        match = 1;
                                        char *w = v; char *end = v + vl;
                                        int word_idx = 0;
                                        while (w < end) {
                                            char *tab = memchr(w, '\t', end - w);
                                            size_t wl = tab ? tab - w : end - w;
                                            if (wl > 0) {
                                                if (word_idx < 64 && (r->bf & query_bfs[word_idx]) != query_bfs[word_idx]) { match = 0; break; }
                                                if (!memmem_pivot(rec_v(r), r->v_len, w, wl) && !memmem_pivot(rec_k(r), r->k_len, w, wl)) { match = 0; break; }
                                                word_idx++;
                                            }
                                            w += wl + 1;
                                        }
                                    }
                                        if (match) {
                                            char weight[16];
                                            int wlen = snprintf(weight, sizeof(weight), "%u\t", 1U << r->weight_log);
                                            size_t rec_len = wlen + r->k_len + 1 + r->v_len + 1;
                                            size_t my_off;
                                            #pragma omp atomic capture
                                            { my_off = out_len; out_len += rec_len; }
                                            if (my_off + rec_len <= OUT_CAP) {
                                                memcpy(out + my_off, weight, wlen); my_off += wlen;
                                                memcpy(out + my_off, rec_k(r), r->k_len); my_off += r->k_len;
                                                out[my_off++] = '\t';
                                                memcpy(out + my_off, rec_v(r), r->v_len); my_off += r->v_len;
                                                out[my_off++] = '\n';
                                            }
                                        }
                                    }
                                }
                                send_response(fd, cipherkey, out_len > OUT_CAP ? 4 : 0, out, out_len > OUT_CAP ? OUT_CAP : out_len);
                                chunk_push(out_c);
                            }
                            else if (op == 2 || op == 3) {
                                int req = op == 2 ? 2 : 4;
                                if (!(perms & req)) { send_response(fd, cipherkey, 1, "denied", 6); chunk_push(c); continue; }
                                int wrote = 0;
                                #pragma omp critical (db)
                                {
                                    if (srv_db_fd < 0) { open_lockfile(db_path); load_db(db_path); srv_db_fd = open_append(db_path); }
                                    wrote = append_raw(srv_db_fd, full_tenant, ft_len, k, kl, v, vl, op == 2 ? OP_PUT : OP_DEL);
                                    if (wrote) sync_fd(srv_db_fd);
                                    load_db(db_path);
                                }
                                if (wrote) {
                                    char receipt[64];
                                    int receipt_len = snprintf(receipt, sizeof(receipt), "%llu", (unsigned long long)valid_size);
                                    send_response(fd, cipherkey, 0, receipt, receipt_len);
                                } else send_response(fd, cipherkey, 3, "shed", 4);
                            }
                            else if (op == 6) {
                                if (!(perms & 1)) { send_response(fd, cipherkey, 1, "denied", 6); chunk_push(c); continue; }
                                Chunk *out_c = chunk_pop();
                                if (!out_c) { send_response(fd, cipherkey, 3, "shed", 4); chunk_push(c); continue; }
                                uint8_t *out = out_c->data; size_t out_len = 0; int more = 0;
                                char v_null[64]; size_t vln = vl > 63 ? 63 : vl; memcpy(v_null, v, vln); v_null[vln] = 0;
                                uint64_t off = strtoull(v_null, NULL, 10);

                                #pragma omp critical (db)
                                {
                                    load_db(db_path);
                                    if (off > valid_size) off = valid_size;
                                    while (off < valid_size) {
                                        if (!rec_valid(off)) break;
                                        Record *r = rec_at(off);
                                        uint64_t next = off + r->len;
                                        if (out_len + r->k_len + r->v_len + 256 > OUT_CAP) { more = 1; break; }
                                        char color_prefix[32];
                                        int cpl = sprintf(color_prefix, "%d:", color);
                                        if (r->t_len >= cpl && !memcmp(rec_t(r), color_prefix, cpl)) {
                                            char line[128];
                                            int ll = sprintf(line, "%llu\t%llu\t%s\t%u\t", (unsigned long long)next, (unsigned long long)off, r->op == OP_PUT ? "put" : "del", 1U << r->weight_log);
                                            memcpy(out + out_len, line, ll); out_len += ll;
                                            memcpy(out + out_len, rec_t(r) + cpl, r->t_len - cpl); out_len += r->t_len - cpl;
                                            out[out_len++] = '\t';
                                            memcpy(out + out_len, rec_k(r), r->k_len); out_len += r->k_len;
                                            out[out_len++] = '\t';
                                            if (r->op == OP_PUT) { memcpy(out + out_len, rec_v(r), r->v_len); out_len += r->v_len; }
                                            out[out_len++] = '\n';
                                        }
                                        off = next;
                                    }
                                }
                                send_response(fd, cipherkey, more ? 4 : 0, out, out_len);
                                chunk_push(out_c);
                            } else if (op == 7) {
                                if (!(perms & 1)) { send_response(fd, cipherkey, 1, "denied", 6); chunk_push(c); continue; }
                                float (*dot_fn)(const void *, const void *, size_t) = NULL;
                                char type[16] = {0};
                                const char *q_vec = NULL;
                                size_t q_len = 0;
                                if (vl > 0 && vl < 15) { memcpy(type, v, vl); }
                                else if (vl >= 15 && kl < 15) { memcpy(type, k, kl); q_vec = v; q_len = vl; }
                                if (!strcmp(type, "f32")) dot_fn = vec_dot_f32;
                                else if (!strcmp(type, "f16")) dot_fn = vec_dot_f16;
                                else if (!strcmp(type, "i8")) dot_fn = vec_dot_i8;
                                else { send_response(fd, cipherkey, 1, "bad type", 8); chunk_push(c); continue; }

                                const char *best_k = NULL;
                                uint16_t best_k_len = 0;
                                #pragma omp critical (db)
                                {
                                    load_db(db_path);
                                    Node *n = NULL;
                                    if (!q_vec) {
                                        n = ht_get(full_tenant, ft_len, k, kl);
                                        if (n) { Record *r = rec_at(n->off1 - 1); if (r->op != OP_DEL && r->v_len) { q_vec = rec_v(r); q_len = r->v_len; } }
                                    }
                                    if (q_vec && ((!strcmp(type,"f32") && q_len%4==0) || (!strcmp(type,"f16") && q_len%2==0) || (!strcmp(type,"i8")))) {
                                        float best_score = -1e30f;
                                        uint64_t start_idx, end_idx;
                                        ht_tenant_range(full_tenant, ft_len, &start_idx, &end_idx);
                                        #pragma omp parallel num_threads(worker_threads())
                                        {
                                            float local_best = -1e30f;
                                            const char *local_k = NULL;
                                            uint16_t local_k_len = 0;
                                            #pragma omp for schedule(dynamic, 1024)
                                            for (uint64_t i = start_idx; i < end_idx; i++) {
                                                Record *c_rec = rec_at(ht[i].off1 - 1);
                                                if (c_rec->op == OP_DEL || c_rec->t_len != ft_len || c_rec->v_len != q_len) continue;
                                                if (n && ht[i].off1 == n->off1) continue;
                                                if (memcmp(rec_t(c_rec), full_tenant, ft_len) != 0) continue;
                                                size_t p_len = q_len > 256 ? 256 : q_len;
                                                float p_score = dot_fn(q_vec, rec_v(c_rec), p_len);
                                                if (p_len < q_len && p_score * ((float)q_len / p_len) < local_best - 0.5f) continue;
                                                float score = p_len < q_len ? p_score + dot_fn(q_vec + p_len, rec_v(c_rec) + p_len, q_len - p_len) : p_score;
                                                if (score > local_best) { local_best = score; local_k = rec_k(c_rec); local_k_len = c_rec->k_len; }
                                            }
                                            #pragma omp critical (best)
                                            {
                                                if (local_best > best_score) { best_score = local_best; best_k = local_k; best_k_len = local_k_len; }
                                            }
                                        }
                                    }
                                }
                                if (best_k) send_response(fd, cipherkey, 0, best_k, best_k_len);
                                else send_response(fd, cipherkey, 2, "not found", 9);
                            } else if (op == 8 || op == 9) {
                                if (!(perms & 1)) { send_response(fd, cipherkey, 1, "denied", 6); chunk_push(c); continue; }
                                double count_est = 0; uint64_t raw_count = 0; double sum = 0; double threshold = 1.0;
                                if (vl > 0 && vl < 64) { char th_buf[64] = {0}; memcpy(th_buf, v, vl); threshold = strtod(th_buf, NULL); if (threshold <= 0 || threshold > 1.0) threshold = 1.0; }
                                #pragma omp critical (db)
                                {
                                    load_db(db_path);
                                    uint64_t start_idx, end_idx;
                                    ht_tenant_range(full_tenant, ft_len, &start_idx, &end_idx);
                                    #pragma omp parallel for reduction(+:count_est,raw_count,sum) schedule(static, 4096) num_threads(worker_threads())
                                    for (uint64_t i = start_idx; i < end_idx; i++) {
                                        Record *r = rec_at(ht[i].off1 - 1);
                                        if (r->op == OP_DEL || r->t_len != ft_len || memcmp(rec_t(r), full_tenant, ft_len)) continue;
                                        if (kl && (r->k_len < kl || memcmp(rec_k(r), k, kl))) continue;
                                if (threshold < 1.0 && (double)(r->key_hash >> 32) * 0x1.0p-32 > threshold) continue;
                                double db_w = (double)(1U << r->weight_log);
                                double qw = 1.0 / threshold;
                                double w = db_w > qw ? db_w : qw;
                                count_est += w;
                                        raw_count++;
                                        if (op == 9 && r->v_len > 0 && r->v_len < 64) {
                                    char buf2[64] = {0}; memcpy(buf2, rec_v(r), r->v_len);
                                    char *t1 = strchr(buf2, '\t'); char *t2 = t1 ? strchr(t1 + 1, '\t') : NULL;
                                    char *p_str = t2 ? t2 + 1 : buf2;
                                    while (*p_str && *p_str != '-' && *p_str != '.' && (*p_str < '0' || *p_str > '9')) p_str++;
                                    sum += strtod(p_str, NULL) * w;
                                        }
                                    }
                                }
                                char val[64]; int vl_out = 0;
                                if (op == 8) vl_out = snprintf(val, sizeof(val), "%.0f\t%llu", count_est, (unsigned long long)raw_count);
                                else vl_out = snprintf(val, sizeof(val), "%.17g\t%llu", sum, (unsigned long long)raw_count);
                                send_response(fd, cipherkey, 0, val, vl_out);
                            } else if (op == 10) {
                                if (!(perms & 2)) { send_response(fd, cipherkey, 1, "denied", 6); chunk_push(c); continue; }
                                char val[64]; int vl_out = 0; int wrote = 0;
                                #pragma omp critical (db)
                                {
                                    if (srv_db_fd < 0) { open_lockfile(db_path); load_db(db_path); srv_db_fd = open_append(db_path); }
                                    long long cur = 0;
                                    Node *n = ht_get(full_tenant, ft_len, k, kl);
                                    if (n) { Record *r = rec_at(n->off1 - 1); if (r->op != OP_DEL && r->v_len < 64) { char buf[64]={0}; memcpy(buf, rec_v(r), r->v_len); cur = strtoll(buf, NULL, 10); } }
                                    char d_buf[64]={0}; memcpy(d_buf, v, vl < 63 ? vl : 63);
                                    long long d = strtoll(d_buf, NULL, 10);
                                    long long next = cur + d;
                                    vl_out = snprintf(val, sizeof(val), "%lld", next);
                                    wrote = append_raw(srv_db_fd, full_tenant, ft_len, k, kl, val, vl_out, OP_PUT);
                                    if (wrote) {
                                        sync_fd(srv_db_fd);
                                        load_db(db_path);
                                    }
                                }
                                if (wrote) send_response(fd, cipherkey, 0, val, vl_out);
                                else send_response(fd, cipherkey, 3, "shed", 4);
                            } else if (op == 11) {
                                char target_buf[32] = {0};
                                char grant_buf[64] = {0};
                                int32_t target = 0, new_pass = 0, grant = 0;
                                if (kl > 30 || vl > 62) { send_response(fd, cipherkey, 1, "bad grant", 9); chunk_push(c); continue; }
                                memcpy(target_buf, k, kl);
                                memcpy(grant_buf, v, vl);
                                if (perms == 0 || sscanf(target_buf, "%d", &target) != 1 || sscanf(grant_buf, "%d,%d", &new_pass, &grant) != 2 || grant < 0 || grant > 7 || (grant & ~perms)) {
                                    send_response(fd, cipherkey, 1, "denied", 6); chunk_push(c); continue;
                                }
                                char user_key[64];
                                char user_val[64];
                                int user_key_len = snprintf(user_key, sizeof(user_key), "%d:%d", target, color);
                                int user_val_len = snprintf(user_val, sizeof(user_val), "%d,%d", new_pass, grant);
                                if (user_key_len <= 0 || user_key_len >= (int)sizeof(user_key) || user_val_len <= 0 || user_val_len >= (int)sizeof(user_val)) {
                                    send_response(fd, cipherkey, 1, "bad grant", 9); chunk_push(c); continue;
                                }
                                int wrote = 0;
                                #pragma omp critical (db)
                                {
                                    if (srv_db_fd < 0) { open_lockfile(db_path); load_db(db_path); srv_db_fd = open_append(db_path); }
                                    wrote = append_raw(srv_db_fd, "0:users", 7, user_key, user_key_len, user_val, user_val_len, OP_PUT);
                                    if (wrote) {
                                        sync_fd(srv_db_fd);
                                        load_db(db_path);
                                    }
                                }
                                if (wrote) send_response(fd, cipherkey, 0, "ok", 2);
                                else send_response(fd, cipherkey, 3, "shed", 4);
                            } else if (op == 12) {
                                if (!(perms & 2)) { send_response(fd, cipherkey, 1, "denied", 6); chunk_push(c); continue; }
                                char val[64]; int vl_out = 0; int ok = 0; int shed = 0;
                                #pragma omp critical (db)
                                {
                                    if (srv_db_fd < 0) { open_lockfile(db_path); load_db(db_path); srv_db_fd = open_append(db_path); }
                                    long long cur = 0;
                                    Node *n = ht_get(full_tenant, ft_len, k, kl);
                                    if (n) { Record *r = rec_at(n->off1 - 1); if (r->op != OP_DEL && r->v_len < 64) { char buf[64]={0}; memcpy(buf, rec_v(r), r->v_len); cur = strtoll(buf, NULL, 10); } }
                                    if (cur > 0) {
                                        vl_out = snprintf(val, sizeof(val), "%lld", cur - 1);
                                        ok = append_raw(srv_db_fd, full_tenant, ft_len, k, kl, val, vl_out, OP_PUT);
                                        if (ok) {
                                            sync_fd(srv_db_fd);
                                            load_db(db_path);
                                        } else {
                                            shed = 1;
                                        }
                                    }
                                }
                                if (ok) send_response(fd, cipherkey, 0, val, vl_out);
                                else if (shed) send_response(fd, cipherkey, 3, "shed", 4);
                                else send_response(fd, cipherkey, 2, "zero", 4);
                            } else if (op == 13) {
                                if (!(perms & 2)) { send_response(fd, cipherkey, 1, "denied", 6); chunk_push(c); continue; }
                                int wrote = 0; int exists = 0;
                                #pragma omp critical (db)
                                {
                                    if (srv_db_fd < 0) { open_lockfile(db_path); load_db(db_path); srv_db_fd = open_append(db_path); }
                                    Node *n = ht_get(full_tenant, ft_len, k, kl);
                                    if (n) { Record *r = rec_at(n->off1 - 1); if (r->op != OP_DEL) exists = 1; }
                                    if (!exists) {
                                        wrote = append_raw(srv_db_fd, full_tenant, ft_len, k, kl, v, vl, OP_PUT);
                                        if (wrote) {
                                            sync_fd(srv_db_fd);
                                            load_db(db_path);
                                        }
                                    }
                                }
                                if (wrote) send_response(fd, cipherkey, 0, v, vl);
                                else if (exists) send_response(fd, cipherkey, 2, "exists", 6);
                                else send_response(fd, cipherkey, 3, "shed", 4);
                            } else if (op == 14) {
                                if (!(perms & 2)) { send_response(fd, cipherkey, 1, "denied", 6); chunk_push(c); continue; }
                                char *tab = memchr(v, '\t', vl);
                                if (!tab) { send_response(fd, cipherkey, 1, "bad arg", 7); chunk_push(c); continue; }
                                size_t old_len = tab - (char*)v;
                                char *new_val = tab + 1;
                                size_t new_len = vl - old_len - 1;
                                int wrote = 0; int match = 0;
                                #pragma omp critical (db)
                                {
                                    if (srv_db_fd < 0) { open_lockfile(db_path); load_db(db_path); srv_db_fd = open_append(db_path); }
                                    Node *n = ht_get(full_tenant, ft_len, k, kl);
                                    if (n) {
                                        Record *r = rec_at(n->off1 - 1);
                                        if (r->op != OP_DEL && r->v_len == old_len && !memcmp(rec_v(r), v, old_len)) match = 1;
                                    }
                                    if (match) {
                                        wrote = append_raw(srv_db_fd, full_tenant, ft_len, k, kl, new_val, new_len, OP_PUT);
                                        if (wrote) {
                                            sync_fd(srv_db_fd);
                                            load_db(db_path);
                                        }
                                    }
                                }
                                if (wrote) send_response(fd, cipherkey, 0, new_val, new_len);
                                else if (match) send_response(fd, cipherkey, 3, "shed", 4);
                                else send_response(fd, cipherkey, 2, "mismatch", 8);
                            } else if (op == 15) {
                                if (!(perms & 2)) { send_response(fd, cipherkey, 1, "denied", 6); chunk_push(c); continue; }
                                char v_null[128]; size_t vln = vl > 127 ? 127 : vl; memcpy(v_null, v, vln); v_null[vln] = 0;
                                char *tab1 = strchr(v_null, '\t');
                                char *tab2 = tab1 ? strchr(tab1 + 1, '\t') : NULL;
                                if (!tab1 || !tab2) { send_response(fd, cipherkey, 1, "bad arg", 7); chunk_push(c); continue; }
                                *tab1 = 0; *tab2 = 0;
                                double hl = parse_f64(v_null);
                                double ts = parse_f64(tab1 + 1);
                                double d = parse_f64(tab2 + 1);
                                if (hl <= 0 || !__builtin_isfinite(hl) || !__builtin_isfinite(ts) || !__builtin_isfinite(d)) {
                                    send_response(fd, cipherkey, 1, "bad arg", 7); chunk_push(c); continue;
                                }

                                char val[192]; int vl_out = 0; int ok = 1; int shed = 0; char err_msg[32] = "error";
                                #pragma omp critical (db)
                                {
                                    if (srv_db_fd < 0) { open_lockfile(db_path); load_db(db_path); srv_db_fd = open_append(db_path); }
                                    double stored_hl = hl, last = ts, value = 0;
                                    int had = 0;
                                    Node *n = ht_get(full_tenant, ft_len, k, kl);
                                    if (n) {
                                        Record *r = rec_at(n->off1 - 1);
                                        if (r->op != OP_DEL && r->v_len < 192) {
                                            char buf[192] = {0}; memcpy(buf, rec_v(r), r->v_len);
                                            char *t1 = strchr(buf, '\t'); char *t2 = t1 ? strchr(t1 + 1, '\t') : NULL;
                                            if (t1 && t2) {
                                                *t1 = 0; *t2 = 0;
                                                stored_hl = strtod(buf, NULL); last = strtod(t1 + 1, NULL); value = strtod(t2 + 1, NULL);
                                                had = 1;
                                            }
                                        }
                                    }
                                    if (had && stored_hl != hl) { ok = 0; strcpy(err_msg, "hl changed"); }
                                    else if (had && ts < last) { ok = 0; strcpy(err_msg, "time reversed"); }
                                    else {
                                        double next = value * __builtin_exp2((last - ts) / hl) + d;
                                        if (!__builtin_isfinite(next)) { ok = 0; strcpy(err_msg, "not finite"); }
                                        else if (__builtin_fabs(next) < 1e-12) {
                                            if (had) {
                                                ok = append_raw(srv_db_fd, full_tenant, ft_len, k, kl, NULL, 0, OP_DEL);
                                                if (ok) sync_fd(srv_db_fd);
                                                else shed = 1;
                                            }
                                            strcpy(val, "0"); vl_out = 1;
                                        } else {
                                            vl_out = snprintf(val, sizeof(val), "%.17g\t%.17g\t%.17g", hl, ts, next);
                                            ok = append_raw(srv_db_fd, full_tenant, ft_len, k, kl, val, vl_out, OP_PUT);
                                            if (ok) sync_fd(srv_db_fd);
                                            else shed = 1;
                                        }
                                        if (ok) load_db(db_path);
                                    }
                                }
                                if (ok) send_response(fd, cipherkey, 0, val, vl_out);
                                else if (shed) send_response(fd, cipherkey, 3, "shed", 4);
                                else send_response(fd, cipherkey, 2, err_msg, strlen(err_msg));
                            } else if (op == 16) {
                                if (!(perms & 2)) { send_response(fd, cipherkey, 1, "denied", 6); chunk_push(c); continue; }
                                int wrote = 0;
                                #pragma omp critical (db)
                                {
                                    if (srv_db_fd < 0) { open_lockfile(db_path); load_db(db_path); srv_db_fd = open_append(db_path); }
                                    char *p = v; char *end = v + vl;
                                    while (p < end) {
                                        char *nl = memchr(p, '\n', end - p);
                                        size_t line_len = nl ? (size_t)(nl - p) : (size_t)(end - p);
                                        char *tab = memchr(p, '\t', line_len);
                                        if (tab) {
                                            size_t b_kl = tab - p;
                                            size_t b_vl = line_len - b_kl - 1;
                                            if (b_kl <= UINT16_MAX && b_vl <= UINT32_MAX) {
                                                if (append_raw(srv_db_fd, full_tenant, ft_len, p, b_kl, tab + 1, b_vl, OP_PUT)) wrote++;
                                            }
                                        }
                                        p += line_len + (nl ? 1 : 0);
                                    }
                                    if (wrote) sync_fd(srv_db_fd);
                                    load_db(db_path);
                                }
                                char res[32]; int rl = snprintf(res, sizeof(res), "%d", wrote);
                                send_response(fd, cipherkey, 0, res, rl);
                            } else {
                                send_response(fd, cipherkey, 1, "bad op", 6);
                            }
                            chunk_push(c);
                        }
                        close(fd);
                        active_conn--;
                    }
                }
            }
        }
    }
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
    fputs("  search TENANT WORD...\n", stderr);
    fputs("  tail [-f] [OFFSET]\n", stderr);
    fputs("  verify\n", stderr);
    fputs("  incr TENANT KEY DELTA\n", stderr);
    fputs("  take TENANT KEY\n", stderr);
    fputs("  decay TENANT KEY HALF_LIFE NOW DELTA\n", stderr);
    fputs("  batch TENANT     # stdin: key<TAB>value\n", stderr);
    fputs("  compact\n", stderr);
    fputs("  closest TYPE TENANT KEY\n", stderr);
    fputs("  serve CIPHERKEY     # always port 51515\n", stderr);
    exit(2);
}

int main(int argc, char **argv)
{
    if (argc < 3) {
        usage(argv[0]);
    }

    const char *db = argv[1];
    const char *cmd = argv[2];

    if (!strcmp(cmd, "serve") && argc == 4) {
        do_serve(db, 51515, atoi(argv[3]));
        return 0;
    }

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

            load_db(db);
            if (!strcmp(args[0], "get") && n == 3) do_get(args[1], args[2]);
            else if (!strcmp(args[0], "put") && n == 4) { append_fd(write_fd, args[1], args[2], args[3], OP_PUT); puts("ok"); }
            else if (!strcmp(args[0], "del") && n == 3) { append_fd(write_fd, args[1], args[2], NULL, OP_DEL); puts("ok"); }
            else if (!strcmp(args[0], "scan") && n >= 2) do_scan(args[1], n == 3 ? args[2] : NULL);
            else if (!strcmp(args[0], "search") && n >= 3) do_search(args[1], n, args);
            else if (!strcmp(args[0], "closest") && n == 4) do_closest(db, args[1], args[2], args[3]);
            else if (!strcmp(args[0], "count") && n >= 2) {
                double count_est = 0; uint64_t raw_c = 0;
                size_t tl = strlen(args[1]);
                size_t pl = n >= 3 ? strlen(args[2]) : 0;
                double threshold = n >= 4 ? strtod(args[3], NULL) : 1.0;
            uint64_t start_idx, end_idx; ht_tenant_range(args[1], tl, &start_idx, &end_idx);
            #pragma omp parallel for reduction(+:count_est,raw_c) schedule(static, 4096) num_threads(worker_threads())
            for (uint64_t i = start_idx; i < end_idx; i++) {
                Record *r = rec_at(ht[i].off1 - 1);
                if (r->op == OP_DEL || r->t_len != tl || memcmp(rec_t(r), args[1], tl)) continue;
                    if (pl && (r->k_len < pl || memcmp(rec_k(r), args[2], pl))) continue;
            if (threshold < 1.0 && (double)(r->key_hash >> 32) * 0x1.0p-32 > threshold) continue;
            double db_w = (double)(1U << r->weight_log);
            double qw = 1.0 / threshold;
            count_est += db_w > qw ? db_w : qw;
            raw_c++;
                }
                printf("%.0f\t%llu\n", count_est, (unsigned long long)raw_c);
            }
            else if (!strcmp(args[0], "sum") && n >= 2) {
                double s = 0; uint64_t raw_s = 0;
                size_t tl = strlen(args[1]);
                size_t pl = n >= 3 ? strlen(args[2]) : 0;
                double threshold = n >= 4 ? strtod(args[3], NULL) : 1.0;
            uint64_t start_idx, end_idx; ht_tenant_range(args[1], tl, &start_idx, &end_idx);
            #pragma omp parallel for reduction(+:s,raw_s) schedule(static, 4096) num_threads(worker_threads())
            for (uint64_t i = start_idx; i < end_idx; i++) {
                Record *r = rec_at(ht[i].off1 - 1);
                if (r->op == OP_DEL || r->t_len != tl || memcmp(rec_t(r), args[1], tl)) continue;
                    if (pl && (r->k_len < pl || memcmp(rec_k(r), args[2], pl))) continue;
            if (threshold < 1.0 && (double)(r->key_hash >> 32) * 0x1.0p-32 > threshold) continue;
            double db_w = (double)(1U << r->weight_log);
            double qw = 1.0 / threshold;
            double w = db_w > qw ? db_w : qw;
            raw_s++;
                    if (r->v_len > 0 && r->v_len < 64) {
                char buf[64] = {0};
                memcpy(buf, rec_v(r), r->v_len);
                char *t1 = strchr(buf, '\t'); char *t2 = t1 ? strchr(t1 + 1, '\t') : NULL;
                char *p_str = t2 ? t2 + 1 : buf;
                while (*p_str && *p_str != '-' && *p_str != '.' && (*p_str < '0' || *p_str > '9')) p_str++;
                s += strtod(p_str, NULL) * w;
                    }
                }
                printf("%.17g\t%llu\n", s, (unsigned long long)raw_s);
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
    if (!strcmp(cmd, "search") && argc >= 5) {
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
