//bin/sh -c 'o=${0%.c}; [ "$o" -nt "$0" ] || ${CC:-gcc} -O3 -march=native -DNDEBUG -fopenmp "$0" -o "$o" || exit; exec "$o" "$@"' "$0" "$@"; exit
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
#include <time.h>
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
    if (len > 65536) {
        return UINT64_MAX;
    }

    uint64_t bf = 0;
    const unsigned char *p = (const unsigned char *)data;
    for (size_t i = 0; i + 2 < len; i++) {
        uint64_t gram = p[i] | ((uint64_t)p[i+1] << 8) | ((uint64_t)p[i+2] << 16);
        gram ^= FNV0;
        gram *= 0xff51afd7ed558ccdULL;
        gram ^= gram >> 33;
        gram *= 0xc4ceb9fe1a85ec53ULL;
        gram ^= gram >> 33;
        bf |= 1ULL << (gram & 63);
        bf |= 1ULL << ((gram >> 32) & 63);
    }
    return bf;
}

typedef struct {
    uint64_t hash;
    uint64_t off1;
    int8_t dist;
} Node;

static uint8_t *map_base;
static size_t map_size;
static size_t valid_size;
static Node *ht;
static uint64_t ht_cap;
static uint64_t ht_len;
static uint64_t ht_sorted_len;
static int8_t ht_max_probe;

static void ht_put(uint64_t hash, uint64_t off);

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
    if (r->weight_log > 31) {
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



static uint64_t get_mem_avail(uint64_t *total_out) {
    struct sysinfo si;
    if (sysinfo(&si)) return 0;
    if (total_out) *total_out = (uint64_t)si.totalram * si.mem_unit;
    int fd = open("/proc/meminfo", O_RDONLY | O_CLOEXEC);
    if (fd >= 0) {
        char buf[2048];
        ssize_t n = read(fd, buf, sizeof(buf) - 1);
        close(fd);
        if (n > 0) {
            buf[n] = 0;
            char *p = strstr(buf, "MemAvailable:");
            if (p) return strtoull(p + 13, NULL, 10) * 1024ULL;
        }
    }
    return (uint64_t)(si.freeram + si.bufferram) * si.mem_unit;
}

static void reserve_ram(size_t bytes)
{
    uint64_t total = 0;
    uint64_t freeish = get_mem_avail(&total);
    if (total && freeish < total / 10 + bytes) {
        diex("ram reserve below 10 percent");
    }
}

static int worker_threads(void)
{
    if (omp_get_active_level() > 1) {
        return 1;
    }

    int n = omp_get_num_procs();
    int keep = (n + 9) / 10;
    return n > keep ? n - keep : 1;
}

static uint64_t ht_target_cap(uint64_t need)
{
    uint64_t ncap = 256;
    while (ncap < need) {
        if (ncap > UINT64_MAX / 2) {
            diex("ht too large");
        }
        ncap *= 2;
    }
    return ncap;
}

static void ht_reserve(uint64_t need)
{
    if (need <= ht_cap * 3 / 4) return;

    uint64_t ncap = ht_target_cap(need * 4 / 3);
    if (ncap <= ht_cap) return;

    int8_t max_probe = 0;
    uint64_t x = ncap;
    while (x) { max_probe++; x >>= 1; }

    size_t bytes = (ncap + max_probe) * sizeof(*ht);
    if (bytes / sizeof(*ht) != (ncap + max_probe)) {
        diex("ht too large");
    }
    reserve_ram(bytes);
    Node *nht = mmap(NULL, bytes, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    if (nht == MAP_FAILED) {
        die("mmap ht");
    }
    madvise(nht, bytes, MADV_HUGEPAGE);
    for (uint64_t i = 0; i < ncap + max_probe; i++) {
        nht[i].dist = -1;
    }

    Node *oht = ht;
    uint64_t old_cap = ht_cap;
    int8_t old_mp = ht_max_probe;

    ht = nht;
    ht_cap = ncap;
    ht_max_probe = max_probe;
    ht_len = 0;
    ht_sorted_len = 0;

    if (oht) {
        for (uint64_t i = 0; i < old_cap + old_mp; i++) {
            if (oht[i].dist >= 0) {
                ht_put(oht[i].hash, oht[i].off1 - 1);
            }
        }
        munmap(oht, (old_cap + old_mp) * sizeof(*oht));
    }
}

static void ht_put(uint64_t hash, uint64_t off)
{
    ht_reserve(ht_len + 1);

    Node elem = {hash, off + 1, 0};
    uint64_t mask = ht_cap - 1;
    uint64_t idx = hash & mask;

    while (1) {
        if (elem.dist >= ht_max_probe) {
            ht_reserve(ht_cap);
            elem.dist = 0;
            idx = elem.hash & (ht_cap - 1);
            continue;
        }
        if (ht[idx].dist < 0) {
            ht[idx] = elem;
            ht_len++;
            return;
        }

        if (ht[idx].hash == elem.hash) {
            Record *r1 = rec_at(ht[idx].off1 - 1);
            Record *r2 = rec_at(elem.off1 - 1);
            if (r1->t_len == r2->t_len && r1->k_len == r2->k_len &&
                !memcmp(rec_t(r1), rec_t(r2), r1->t_len) &&
                !memcmp(rec_k(r1), rec_k(r2), r1->k_len)) {

                ht[idx] = elem;
                return;
            }
        }

        if (ht[idx].dist < elem.dist) {
            Node tmp = ht[idx];
            ht[idx] = elem;
            elem = tmp;
        }

        idx++;
        elem.dist++;
    }
}



static int cmp_node(const void *a, const void *b) {
    const Node *x = a, *y = b;
    if (x->hash != y->hash) return x->hash < y->hash ? -1 : 1;
    return x->off1 < y->off1 ? -1 : 1;
}

static int cmp_node_key(const void *a, const void *b) {
    const Node *x = a, *y = b;
    Record *rx = rec_at(x->off1 - 1);
    Record *ry = rec_at(y->off1 - 1);
    if (rx->t_len != ry->t_len) return rx->t_len < ry->t_len ? -1 : 1;
    if (rx->k_len != ry->k_len) return rx->k_len < ry->k_len ? -1 : 1;
    int c1 = memcmp(rec_t(rx), rec_t(ry), rx->t_len);
    if (c1) return c1;
    int c2 = memcmp(rec_k(rx), rec_k(ry), rx->k_len);
    if (c2) return c2;
    return x->off1 > y->off1 ? -1 : (x->off1 < y->off1 ? 1 : 0);
}

static void deduplicate_ht(void) {
    if (!ht_len) { ht_sorted_len = 0; return; }
    if (ht_len > ht_sorted_len) {
        qsort(ht, ht_len, sizeof(*ht), cmp_node);
    }
    uint64_t out = 0;
    for (uint64_t i = 0; i < ht_len; ) {
        uint64_t j = i + 1;
        while (j < ht_len && ht[j].hash == ht[i].hash) j++;
        if (j - i > 1) {
            qsort(ht + i, j - i, sizeof(*ht), cmp_node_key);
            for (uint64_t k = i; k < j; k++) {
                if (k > i) {
                    Record *rk = rec_at(ht[k].off1 - 1);
                    Record *rp = rec_at(ht[k-1].off1 - 1);
                    if (rk->t_len == rp->t_len && rk->k_len == rp->k_len &&
                        !memcmp(rec_t(rk), rec_t(rp), rk->t_len) &&
                        !memcmp(rec_k(rk), rec_k(rp), rk->k_len)) {
                        continue;
                    }
                }
                ht[out++] = ht[k];
            }
        } else {
            ht[out++] = ht[i];
        }
        i = j;
    }
    ht_len = out;
    ht_sorted_len = out;
}


static uint64_t ht_lower_bound(uint64_t hash) {
    uint64_t lo = 0, hi = ht_sorted_len;
    while (lo < hi) {
        uint64_t mid = lo + (hi - lo) / 2;
        if (ht[mid].hash < hash) lo = mid + 1;
        else hi = mid;
    }
    return lo;
}

static void ht_tenant_range(const char *t, size_t tl, uint64_t *start, uint64_t *end) {
    uint64_t th = fnv_bytes(FNV0, t, tl);
    uint64_t tenant_bits = th << 32;
    uint32_t upper32 = (uint32_t)(tenant_bits >> 32) + 1;
    *start = ht_lower_bound(tenant_bits);
    *end = upper32 ? ht_lower_bound((uint64_t)upper32 << 32) : ht_sorted_len;
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

static ino_t map_ino;
static void load_db(const char *path)
{
    struct stat st;
    if (stat(path, &st)) return;
    if (map_base && (st.st_ino != map_ino || st.st_size < (off_t)map_size)) {
        munmap(map_base, map_size);
        map_base = NULL; map_size = 0; valid_size = 0; ht_len = 0; ht_sorted_len = 0;
        if (ht) { munmap(ht, (ht_cap + ht_max_probe) * sizeof(*ht)); ht = NULL; ht_cap = 0; ht_max_probe = 0; }
    }
    map_ino = st.st_ino;
    if (st.st_size <= (off_t)map_size) return;
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
    (void)posix_madvise(map_base, map_size, POSIX_MADV_NORMAL);
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

    static __thread uint32_t append_chk = 0;
    static __thread uint64_t last_free = UINT64_MAX, last_res = 0;
    if (op != OP_DEL) {
        if (++append_chk % 1024 == 1) {
            struct statvfs st;
            if (!fstatvfs(fd, &st) && st.f_blocks > 0 && st.f_frsize > 0) {
                last_free = (uint64_t)st.f_bavail * st.f_frsize;
                last_res = (uint64_t)((long double)st.f_blocks * st.f_frsize * 0.1L);
            }
        }
        if (last_free < last_res + r.len) return 0;
        if (tl > 0 && t[0] != '0') {
            if (ht_cap) {
                Node *n = ht_get(t, (uint16_t)tl, k, (uint16_t)kl);
                if (n && rec_at(n->off1 - 1)->op != OP_DEL) {
                    r.weight_log = rec_at(n->off1 - 1)->weight_log;
                }
            }
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

static int decay_value_at(const char *s, size_t n, double now, double *out)
{
    if (!n || n >= 192 || s[0] != '\x1F') {
        return 0;
    }

    char buf[192] = {0};
    memcpy(buf, s + 1, n - 1);
    char *t1 = strchr(buf, '\t');
    char *t2 = t1 ? strchr(t1 + 1, '\t') : NULL;
    if (!t1 || !t2) {
        return 0;
    }

    char *e1;
    char *e2;
    char *e3;
    *t1 = 0;
    *t2 = 0;
    double hl = strtod(buf, &e1);
    double last = strtod(t1 + 1, &e2);
    double val = strtod(t2 + 1, &e3);
    if (e1 != t1 || e2 != t2 || *e3 || hl <= 0 || !__builtin_isfinite(hl) || !__builtin_isfinite(last) || !__builtin_isfinite(val)) {
        return 0;
    }

    double age = now >= last ? now - last : 0;
    double x = age ? val * __builtin_exp2(-age / hl) : val;
    double floor = __builtin_fabs(val) * 0x1.0p-20;
    if (floor < 0.001) {
        floor = 0.001;
    }
    if (age > hl * 64 || __builtin_fabs(x) < floor) {
        *out = 0;
        return 1;
    }
    *out = x;
    return 1;
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
    if (r->v_len >= 192 || r->v_len == 0 || rec_v(r)[0] != '\x1F') {
        diex("stored decay state malformed");
    }

    char buf[192] = {0};
    memcpy(buf, rec_v(r) + 1, r->v_len - 1);
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
    if (had && stored_hl != hl) {
        close(lockfd);
        diex("decay half life changed");
    }

    double next;
    if (had && ts < last) {
        next = value + d * __builtin_exp2((ts - last) / hl);
        ts = last;
    } else {
        next = value * __builtin_exp2((last - ts) / hl) + d;
    }
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
    snprintf(val, sizeof(val), "\x1F%.17g\t%.17g\t%.17g", hl, ts, next);

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
    uint64_t n_puts = 0;
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
        n_puts += r->op == OP_PUT;
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
    printf("puts\t%llu\n", (unsigned long long)n_puts);
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

static void term_lens(int n, char **words, size_t *lens)
{
    for (int i = 0; i < n; i++) lens[i] = strlen(words[i]);
    for (int i = 0; i + 1 < n; i++) {
        for (int j = i + 1; j < n; j++) {
            if (lens[j] <= lens[i]) continue;
            size_t nl = lens[i]; lens[i] = lens[j]; lens[j] = nl;
            char *na = words[i]; words[i] = words[j]; words[j] = na;
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

__attribute__((target("avx512bw")))
static const void *memmem_pivot_scan_avx512(const unsigned char *hay, size_t hay_len, const unsigned char *need, size_t needle_len, size_t pivot)
{
    const unsigned char *p = hay + pivot;
    const unsigned char *end = hay + hay_len - (needle_len - pivot) + 1;
    __m512i pv = _mm512_set1_epi8((char)need[pivot]);

    while (p + 64 <= end) {
        uint64_t mask = (uint64_t)_mm512_cmpeq_epi8_mask(_mm512_loadu_si512((const void *)p), pv);
        while (mask) {
            uint32_t bit = (uint32_t)__builtin_ctzll(mask);
            const unsigned char *m = p + bit - pivot;
            if (!memcmp(m, need, needle_len)) {
                return m;
            }
            mask &= mask - 1;
        }
        p += 64;
    }

    while (p < end) {
        p = memchr(p, need[pivot], end - p);
        if (!p) {
            return NULL;
        }
        if (!memcmp(p - pivot, need, needle_len)) {
            return p - pivot;
        }
        p++;
    }
    return NULL;
}

__attribute__((target("avx2")))
static const void *memmem_pivot_scan_avx2(const unsigned char *hay, size_t hay_len, const unsigned char *need, size_t needle_len, size_t pivot)
{
    const unsigned char *p = hay + pivot;
    const unsigned char *end = hay + hay_len - (needle_len - pivot) + 1;
    __m256i pv = _mm256_set1_epi8((char)need[pivot]);

    while (p + 32 <= end) {
        uint32_t mask = (uint32_t)_mm256_movemask_epi8(_mm256_cmpeq_epi8(_mm256_loadu_si256((const __m256i *)(const void *)p), pv));
        while (mask) {
            uint32_t bit = (uint32_t)__builtin_ctz(mask);
            const unsigned char *m = p + bit - pivot;
            if (!memcmp(m, need, needle_len)) {
                return m;
            }
            mask &= mask - 1;
        }
        p += 32;
    }

    while (p < end) {
        p = memchr(p, need[pivot], end - p);
        if (!p) {
            return NULL;
        }
        if (!memcmp(p - pivot, need, needle_len)) {
            return p - pivot;
        }
        p++;
    }
    return NULL;
}

static const void *memmem_pivot(const void *haystack, size_t hay_len, const void *needle, size_t needle_len)
{
    const unsigned char *hay = haystack;
    const unsigned char *need = needle;
    size_t pivot = 0;
    unsigned best = UINT_MAX;

    if (!needle_len) {
        return hay;
    }
    if (needle_len > hay_len) {
        return NULL;
    }
    if (needle_len == 1) {
        return memchr(hay, need[0], hay_len);
    }

    for (size_t i = 0; i < needle_len; i++) {
        unsigned rank = byte_rank(need[i]);
        if (rank < best) {
            best = rank;
            pivot = i;
        }
    }

    if (__builtin_cpu_supports("avx512bw")) {
        return memmem_pivot_scan_avx512(hay, hay_len, need, needle_len, pivot);
    }
    return memmem_pivot_scan_avx2(hay, hay_len, need, needle_len, pivot);
}

static int cmp_u64(const void *a, const void *b) {
    uint64_t x = *(const uint64_t *)a, y = *(const uint64_t *)b;
    return (x > y) - (x < y);
}

static void radix_sort_u64(uint64_t *a, size_t n) {
    if (!n) return;
    uint64_t *b = mmap(NULL, n * 8, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    if (b == MAP_FAILED) { qsort(a, n, sizeof(uint64_t), cmp_u64); return; }
    for (int shift = 0; shift < 64; shift += 8) {
        size_t cnt[256] = {0};
        for (size_t i = 0; i < n; i++) cnt[(a[i] >> shift) & 255]++;
        size_t pos[256], p = 0;
        for (int i = 0; i < 256; i++) { pos[i] = p; p += cnt[i]; }
        for (size_t i = 0; i < n; i++) b[pos[(a[i] >> shift) & 255]++] = a[i];
        uint64_t *tmp = a; a = b; b = tmp;
    }
    munmap(b, n * 8);
}

static uint64_t *get_sorted_offs(uint64_t start_idx, uint64_t count) {
    if (!count || count > SIZE_MAX / 8) return NULL;
    size_t bytes = count * 8;
    uint64_t total = 0;
    uint64_t freeish = get_mem_avail(&total);
    if (total && freeish < total / 10 + bytes) return NULL;
    uint64_t *offs = mmap(NULL, bytes, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    if (offs != MAP_FAILED) {
        for (uint64_t i = 0; i < count; i++) offs[i] = ht[start_idx + i].off1 - 1;
        radix_sort_u64(offs, count);
        return offs;
    }
    return NULL;
}

static int rec_has_terms(Record *r, int num_words, char **words, size_t *lens, uint64_t *term_bfs, double now)
{
    int is_decay_val = 0;
    double decay_cur = 1;
    if (r->v_len > 0 && r->v_len < 192) {
        is_decay_val = decay_value_at(rec_v(r), r->v_len, now, &decay_cur);
        if (is_decay_val && decay_cur == 0) return 0;
    }

    for (int j = 0; j < num_words; j++) {
        int inv = words[j][0] == '-';
        char *kw = words[j] + inv;
        size_t kwl = lens[j] - inv;
        if (kwl == 0) continue;
        int bf_match = (r->bf & term_bfs[j]) == term_bfs[j];
        int found = 0;
        if (bf_match) found = (!is_decay_val && memmem_pivot(rec_v(r), r->v_len, kw, kwl)) || memmem_pivot(rec_k(r), r->k_len, kw, kwl);
        if (inv ? found : !found) return 0;
    }
    return 1;
}

static void write_all(int fd, const void *p, size_t n);
static void write_weighted_record(Record *r, double now);

static int do_search(const char *t, int num_words, char **words)
{
    size_t tl = strlen(t);
    if (tl > UINT16_MAX || !ht_cap) return 0;

    size_t lens[num_words];
    term_lens(num_words, words, lens);
    for (int i = 0; i < num_words; i++) {
        if (lens[i] < 3) diex("search terms need at least 3 bytes");
    }

    uint64_t term_bfs[num_words];
    for (int i = 0; i < num_words; i++) {
        int inv = words[i][0] == '-';
        if (lens[i] > (size_t)inv) term_bfs[i] = compute_bf(words[i] + inv, lens[i] - inv);
        else term_bfs[i] = 0;
    }

    double now = (double)time(NULL);
    uint64_t start_idx, end_idx;
    ht_tenant_range(t, tl, &start_idx, &end_idx);
    uint64_t count = end_idx > start_idx ? end_idx - start_idx : 0;
    uint64_t *offs = get_sorted_offs(start_idx, count);
    #pragma omp parallel for schedule(static, 4096) num_threads(worker_threads())
    for (uint64_t i = 0; i < count; i++) {
        Record *r = rec_at(offs ? offs[i] : (ht[start_idx + i].off1 - 1));
        if (r->op == OP_DEL || r->t_len != tl || memcmp(rec_t(r), t, tl)) continue;
        if (!rec_has_terms(r, num_words, words, lens, term_bfs, now)) continue;
        #pragma omp critical
        {
            write_weighted_record(r, now);
        }
    }
    if (offs) munmap(offs, count * 8);
    return 0;
}

static void write_weighted_record(Record *r, double now)
{
    const char *v = rec_v(r);
    size_t vl = r->v_len;
    char buf[64];
    double cur = 0;

    if (vl > 0 && vl < 192 && decay_value_at(v, vl, now, &cur)) {
        if (cur == 0) {
            return;
        }
        int n = snprintf(buf, sizeof(buf), "%.17g", cur);
        if (n < 0 || n >= (int)sizeof(buf)) {
            diex("decay value print failed");
        }
        v = buf;
        vl = (size_t)n;
    }

    double print_w = (double)(1U << (r->weight_log > 13 ? 13 : r->weight_log));
    if (cur) print_w *= __builtin_fabs(cur);
    if (print_w < 0.5) return;
    char wbuf[32];
    int wl = snprintf(wbuf, sizeof(wbuf), "%.5g\t", print_w);
    if (wl < 0 || wl >= (int)sizeof(wbuf)) diex("weight print failed");
    char lbuf[8192];
    size_t total = (size_t)wl + r->k_len + 1 + vl + 1;
    if (total <= sizeof(lbuf)) {
        size_t off = 0;
        memcpy(lbuf, wbuf, wl); off = wl;
        memcpy(lbuf + off, rec_k(r), r->k_len); off += r->k_len;
        lbuf[off++] = '\t';
        memcpy(lbuf + off, v, vl); off += vl;
        lbuf[off++] = '\n';
        write_all(1, lbuf, off);
    } else {
        write_all(1, wbuf, wl);
        write_all(1, rec_k(r), r->k_len);
        write_all(1, "\t", 1);
        write_all(1, v, vl);
        write_all(1, "\n", 1);
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

    double now = (double)time(NULL);
    uint64_t start_idx, end_idx;
    ht_tenant_range(t, tl, &start_idx, &end_idx);
    uint64_t count = end_idx > start_idx ? end_idx - start_idx : 0;
    uint64_t *offs = get_sorted_offs(start_idx, count);
    #pragma omp parallel for schedule(static, 4096) num_threads(worker_threads())
    for (uint64_t i = 0; i < count; i++) {
        Record *r = rec_at(offs ? offs[i] : (ht[start_idx + i].off1 - 1));
        if (r->op == OP_DEL || r->t_len != tl) {
            continue;
        }
        if (memcmp(rec_t(r), t, tl)) {
            continue;
        }
        if (pl && (r->k_len < pl || memcmp(rec_k(r), prefix, pl))) {
            continue;
        }
        #pragma omp critical
        write_weighted_record(r, now);
    }
    if (offs) munmap(offs, count * 8);
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

    double decay_factor = 0;
    struct statvfs st;
    if (!fstatvfs(fd, &st) && st.f_blocks > 0 && st.f_frsize > 0) {
        uint64_t free_bytes = (uint64_t)st.f_bavail * st.f_frsize;
        uint64_t reserve_bytes = (uint64_t)((long double)st.f_blocks * st.f_frsize * 0.1L);
        uint64_t target = free_bytes > reserve_bytes ? free_bytes - reserve_bytes : 0;
        uint64_t disk_half = (uint64_t)st.f_blocks * st.f_frsize / 2;
        if (target > disk_half) target = disk_half;
        if (live_bytes > target) {
            double low = 0, high = 13.0 / valid_size;
            for (int step = 0; step < 40; step++) {
                double mid = (low + high) / 2;
                uint64_t stride = ht_len / 100000 + 1;
                double est = 0;
                for (uint64_t i = 0; i < ht_len; i += stride) {
                    Record *r = rec_at(ht[i].off1 - 1);
                    if (r->op == OP_DEL) continue;
                    int twl = r->weight_log;
                    if (r->t_len > 0 && rec_t(r)[0] != '0' && !(r->v_len > 0 && r->v_len < 192 && rec_v(r)[0] == '\x1F')) {
                        int ewl = (int)((valid_size - (ht[i].off1 - 1)) * mid);
                        if (ewl > 13) ewl = 13;
                        if (ewl > twl) twl = ewl;
                    }
                    est += (double)(r->len >> (twl - r->weight_log));
                }
                est *= stride;
                if (est > (double)target) low = mid; else high = mid;
            }
            decay_factor = high;
        }
    }

    (void)posix_fadvise(fd, 0, 0, POSIX_FADV_SEQUENTIAL);
    reserve_ram(COMPACT_WRITE_BYTES);
    char *buf = mmap(NULL, COMPACT_WRITE_BYTES, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    if (buf == MAP_FAILED) {
        die("mmap compact");
    }
    madvise(buf, COMPACT_WRITE_BYTES, MADV_HUGEPAGE);

    size_t used = 0;
    double compact_now = (double)time(NULL);
    for (uint64_t off = 0; off < valid_size;) {
        Record *r = rec_at(off);
        Node *n = ht_get(rec_t(r), r->t_len, rec_k(r), r->k_len);
        if (n && (n->off1 - 1) == off && r->op != OP_DEL) {
            int keep = 1;
            uint8_t new_wl = r->weight_log;
            if (decay_factor > 0 && r->t_len > 0 && rec_t(r)[0] != '0' && !(r->v_len > 0 && r->v_len < 192 && rec_v(r)[0] == '\x1F')) {
                int ewl = (int)((valid_size - off) * decay_factor);
                if (ewl > 13) ewl = 13;
                uint8_t twl = r->weight_log > ewl ? r->weight_log : ewl;
                double gate = (double)(r->key_hash & 0xFFFFFFFFULL) * 0x1.0p-32;
                if (gate > 1.0 / (1U << twl)) keep = 0;
                new_wl = twl;
            }
            if (keep) {
                double cur = 0;
                if (r->v_len > 0 && r->v_len < 192 && decay_value_at(rec_v(r), r->v_len, compact_now, &cur) && cur == 0) {
                    keep = 0;
                }
            }
            if (keep) {
                if (COMPACT_WRITE_BYTES - used < r->len) {
                    write_all(fd, buf, used);
                    used = 0;
                }
                if (r->len > COMPACT_WRITE_BYTES) {
                    if (new_wl != r->weight_log) {
                        Record tmp = *r;
                        tmp.weight_log = new_wl;
                        tmp.check = rec_check(&tmp, rec_t(r), rec_k(r), rec_v(r));
                        write_all(fd, &tmp, sizeof(tmp));
                        write_all(fd, rec_t(r), r->len - sizeof(tmp));
                    } else {
                        write_all(fd, r, r->len);
                    }
                } else {
                    memcpy(buf + used, r, r->len);
                    if (new_wl != r->weight_log) {
                        Record *wr = (Record *)(buf + used);
                        wr->weight_log = new_wl;
                        wr->check = rec_check(wr, rec_t(wr), rec_k(wr), rec_v(wr));
                    }
                    used += r->len;
                }
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

__attribute__((target("avx512f")))
static float vec_dot_f32_avx512(const void *a, const void *b, size_t bytes) {
    size_t n = bytes / 4;
    const unaligned_f32 *fa = a, *fb = b;
    __m512 sum0 = _mm512_setzero_ps();
    __m512 sum1 = _mm512_setzero_ps();
    __m512 sum2 = _mm512_setzero_ps();
    __m512 sum3 = _mm512_setzero_ps();
    size_t i = 0;
    for (; i + 63 < n; i += 64) {
        sum0 = _mm512_fmadd_ps(_mm512_loadu_ps(fa + i), _mm512_loadu_ps(fb + i), sum0);
        sum1 = _mm512_fmadd_ps(_mm512_loadu_ps(fa + i + 16), _mm512_loadu_ps(fb + i + 16), sum1);
        sum2 = _mm512_fmadd_ps(_mm512_loadu_ps(fa + i + 32), _mm512_loadu_ps(fb + i + 32), sum2);
        sum3 = _mm512_fmadd_ps(_mm512_loadu_ps(fa + i + 48), _mm512_loadu_ps(fb + i + 48), sum3);
    }
    sum0 = _mm512_add_ps(sum0, sum1);
    sum2 = _mm512_add_ps(sum2, sum3);
    sum0 = _mm512_add_ps(sum0, sum2);
    for (; i < n; i += 16) {
        __mmask16 mask = n - i >= 16 ? 0xFFFF : ((1u << (n - i)) - 1u);
        sum0 = _mm512_fmadd_ps(_mm512_maskz_loadu_ps(mask, fa + i), _mm512_maskz_loadu_ps(mask, fb + i), sum0);
    }
    return _mm512_reduce_add_ps(sum0);
}

__attribute__((target("avx2,fma")))
static float vec_dot_f32_avx2(const void *a, const void *b, size_t bytes) {
    size_t n = bytes / 4;
    const unaligned_f32 *fa = a, *fb = b;
    __m256 sum0 = _mm256_setzero_ps();
    __m256 sum1 = _mm256_setzero_ps();
    __m256 sum2 = _mm256_setzero_ps();
    __m256 sum3 = _mm256_setzero_ps();
    __m256 sum4 = _mm256_setzero_ps();
    __m256 sum5 = _mm256_setzero_ps();
    __m256 sum6 = _mm256_setzero_ps();
    __m256 sum7 = _mm256_setzero_ps();
    size_t i = 0;
    for (; i + 63 < n; i += 64) {
        sum0 = _mm256_fmadd_ps(_mm256_loadu_ps(fa + i), _mm256_loadu_ps(fb + i), sum0);
        sum1 = _mm256_fmadd_ps(_mm256_loadu_ps(fa + i + 8), _mm256_loadu_ps(fb + i + 8), sum1);
        sum2 = _mm256_fmadd_ps(_mm256_loadu_ps(fa + i + 16), _mm256_loadu_ps(fb + i + 16), sum2);
        sum3 = _mm256_fmadd_ps(_mm256_loadu_ps(fa + i + 24), _mm256_loadu_ps(fb + i + 24), sum3);
        sum4 = _mm256_fmadd_ps(_mm256_loadu_ps(fa + i + 32), _mm256_loadu_ps(fb + i + 32), sum4);
        sum5 = _mm256_fmadd_ps(_mm256_loadu_ps(fa + i + 40), _mm256_loadu_ps(fb + i + 40), sum5);
        sum6 = _mm256_fmadd_ps(_mm256_loadu_ps(fa + i + 48), _mm256_loadu_ps(fb + i + 48), sum6);
        sum7 = _mm256_fmadd_ps(_mm256_loadu_ps(fa + i + 56), _mm256_loadu_ps(fb + i + 56), sum7);
    }
    for (; i + 31 < n; i += 32) {
        sum0 = _mm256_fmadd_ps(_mm256_loadu_ps(fa + i), _mm256_loadu_ps(fb + i), sum0);
        sum1 = _mm256_fmadd_ps(_mm256_loadu_ps(fa + i + 8), _mm256_loadu_ps(fb + i + 8), sum1);
        sum2 = _mm256_fmadd_ps(_mm256_loadu_ps(fa + i + 16), _mm256_loadu_ps(fb + i + 16), sum2);
        sum3 = _mm256_fmadd_ps(_mm256_loadu_ps(fa + i + 24), _mm256_loadu_ps(fb + i + 24), sum3);
    }
    sum0 = _mm256_add_ps(sum0, sum4);
    sum1 = _mm256_add_ps(sum1, sum5);
    sum2 = _mm256_add_ps(sum2, sum6);
    sum3 = _mm256_add_ps(sum3, sum7);
    sum0 = _mm256_add_ps(sum0, sum1);
    sum2 = _mm256_add_ps(sum2, sum3);
    sum0 = _mm256_add_ps(sum0, sum2);
    for (; i + 7 < n; i += 8) {
        sum0 = _mm256_fmadd_ps(_mm256_loadu_ps(fa + i), _mm256_loadu_ps(fb + i), sum0);
    }
    float buf[8];
    _mm256_storeu_ps(buf, sum0);
    float sum = buf[0] + buf[1] + buf[2] + buf[3] + buf[4] + buf[5] + buf[6] + buf[7];
    for (; i < n; i++) sum += fa[i] * fb[i];
    return sum;
}

static float vec_dot_f32(const void *a, const void *b, size_t bytes) {
    if (__builtin_cpu_supports("avx512f")) {
        return vec_dot_f32_avx512(a, b, bytes);
    }
    return vec_dot_f32_avx2(a, b, bytes);
}

__attribute__((target("avx512fp16,avx512vl,avx512f")))
static float vec_dot_f16_avx512fp16(const void *a, const void *b, size_t bytes) {
    size_t n = bytes / 2;
    const unaligned_f16 *fa = a, *fb = b;
    __m512h sum0 = _mm512_setzero_ph();
    __m512h sum1 = _mm512_setzero_ph();
    __m512h sum2 = _mm512_setzero_ph();
    __m512h sum3 = _mm512_setzero_ph();
    size_t i = 0;
    for (; i + 127 < n; i += 128) {
        sum0 = _mm512_fmadd_ph(_mm512_castsi512_ph(_mm512_loadu_si512((const void *)(fa + i))), _mm512_castsi512_ph(_mm512_loadu_si512((const void *)(fb + i))), sum0);
        sum1 = _mm512_fmadd_ph(_mm512_castsi512_ph(_mm512_loadu_si512((const void *)(fa + i + 32))), _mm512_castsi512_ph(_mm512_loadu_si512((const void *)(fb + i + 32))), sum1);
        sum2 = _mm512_fmadd_ph(_mm512_castsi512_ph(_mm512_loadu_si512((const void *)(fa + i + 64))), _mm512_castsi512_ph(_mm512_loadu_si512((const void *)(fb + i + 64))), sum2);
        sum3 = _mm512_fmadd_ph(_mm512_castsi512_ph(_mm512_loadu_si512((const void *)(fa + i + 96))), _mm512_castsi512_ph(_mm512_loadu_si512((const void *)(fb + i + 96))), sum3);
    }
    sum0 = _mm512_add_ph(_mm512_add_ph(sum0, sum1), _mm512_add_ph(sum2, sum3));
    for (; i < n; i += 32) {
        size_t rem = n - i;
        __mmask32 mask = rem >= 32 ? 0xFFFFFFFFu : (uint32_t)((1ULL << rem) - 1ULL);
        __m512h va = _mm512_castsi512_ph(_mm512_maskz_loadu_epi16(mask, fa + i));
        __m512h vb = _mm512_castsi512_ph(_mm512_maskz_loadu_epi16(mask, fb + i));
        sum0 = _mm512_fmadd_ph(va, vb, sum0);
    }
    return _mm512_reduce_add_ph(sum0);
}

__attribute__((target("avx2,f16c,fma")))
static float vec_dot_f16_avx2(const void *a, const void *b, size_t bytes) {
    size_t n = bytes / 2;
    const unaligned_f16 *fa = a, *fb = b;
    __m256 sum0 = _mm256_setzero_ps();
    __m256 sum1 = _mm256_setzero_ps();
    __m256 sum2 = _mm256_setzero_ps();
    __m256 sum3 = _mm256_setzero_ps();
    __m256 sum4 = _mm256_setzero_ps();
    __m256 sum5 = _mm256_setzero_ps();
    __m256 sum6 = _mm256_setzero_ps();
    __m256 sum7 = _mm256_setzero_ps();
    size_t i = 0;
    for (; i + 63 < n; i += 64) {
        sum0 = _mm256_fmadd_ps(_mm256_cvtph_ps(_mm_loadu_si128((const __m128i*)(fa + i))), _mm256_cvtph_ps(_mm_loadu_si128((const __m128i*)(fb + i))), sum0);
        sum1 = _mm256_fmadd_ps(_mm256_cvtph_ps(_mm_loadu_si128((const __m128i*)(fa + i + 8))), _mm256_cvtph_ps(_mm_loadu_si128((const __m128i*)(fb + i + 8))), sum1);
        sum2 = _mm256_fmadd_ps(_mm256_cvtph_ps(_mm_loadu_si128((const __m128i*)(fa + i + 16))), _mm256_cvtph_ps(_mm_loadu_si128((const __m128i*)(fb + i + 16))), sum2);
        sum3 = _mm256_fmadd_ps(_mm256_cvtph_ps(_mm_loadu_si128((const __m128i*)(fa + i + 24))), _mm256_cvtph_ps(_mm_loadu_si128((const __m128i*)(fb + i + 24))), sum3);
        sum4 = _mm256_fmadd_ps(_mm256_cvtph_ps(_mm_loadu_si128((const __m128i*)(fa + i + 32))), _mm256_cvtph_ps(_mm_loadu_si128((const __m128i*)(fb + i + 32))), sum4);
        sum5 = _mm256_fmadd_ps(_mm256_cvtph_ps(_mm_loadu_si128((const __m128i*)(fa + i + 40))), _mm256_cvtph_ps(_mm_loadu_si128((const __m128i*)(fb + i + 40))), sum5);
        sum6 = _mm256_fmadd_ps(_mm256_cvtph_ps(_mm_loadu_si128((const __m128i*)(fa + i + 48))), _mm256_cvtph_ps(_mm_loadu_si128((const __m128i*)(fb + i + 48))), sum6);
        sum7 = _mm256_fmadd_ps(_mm256_cvtph_ps(_mm_loadu_si128((const __m128i*)(fa + i + 56))), _mm256_cvtph_ps(_mm_loadu_si128((const __m128i*)(fb + i + 56))), sum7);
    }
    for (; i + 31 < n; i += 32) {
        sum0 = _mm256_fmadd_ps(_mm256_cvtph_ps(_mm_loadu_si128((const __m128i*)(fa + i))), _mm256_cvtph_ps(_mm_loadu_si128((const __m128i*)(fb + i))), sum0);
        sum1 = _mm256_fmadd_ps(_mm256_cvtph_ps(_mm_loadu_si128((const __m128i*)(fa + i + 8))), _mm256_cvtph_ps(_mm_loadu_si128((const __m128i*)(fb + i + 8))), sum1);
        sum2 = _mm256_fmadd_ps(_mm256_cvtph_ps(_mm_loadu_si128((const __m128i*)(fa + i + 16))), _mm256_cvtph_ps(_mm_loadu_si128((const __m128i*)(fb + i + 16))), sum2);
        sum3 = _mm256_fmadd_ps(_mm256_cvtph_ps(_mm_loadu_si128((const __m128i*)(fa + i + 24))), _mm256_cvtph_ps(_mm_loadu_si128((const __m128i*)(fb + i + 24))), sum3);
    }
    sum0 = _mm256_add_ps(sum0, sum4);
    sum1 = _mm256_add_ps(sum1, sum5);
    sum2 = _mm256_add_ps(sum2, sum6);
    sum3 = _mm256_add_ps(sum3, sum7);
    sum0 = _mm256_add_ps(sum0, sum1);
    sum2 = _mm256_add_ps(sum2, sum3);
    sum0 = _mm256_add_ps(sum0, sum2);
    for (; i + 7 < n; i += 8) {
        sum0 = _mm256_fmadd_ps(_mm256_cvtph_ps(_mm_loadu_si128((const __m128i*)(fa + i))), _mm256_cvtph_ps(_mm_loadu_si128((const __m128i*)(fb + i))), sum0);
    }
    float buf[8];
    _mm256_storeu_ps(buf, sum0);
    float sum = buf[0] + buf[1] + buf[2] + buf[3] + buf[4] + buf[5] + buf[6] + buf[7];
    for (; i < n; i++) {
        __m128i ha = _mm_insert_epi16(_mm_setzero_si128(), fa[i], 0);
        __m128i hb = _mm_insert_epi16(_mm_setzero_si128(), fb[i], 0);
        sum += _mm_cvtss_f32(_mm_cvtph_ps(ha)) * _mm_cvtss_f32(_mm_cvtph_ps(hb));
    }
    return sum;
}

static float vec_dot_f16(const void *a, const void *b, size_t bytes) {
    if (__builtin_cpu_supports("avx512fp16")) {
        return vec_dot_f16_avx512fp16(a, b, bytes);
    }
    return vec_dot_f16_avx2(a, b, bytes);
}

__attribute__((target("avx512vnni,avx512bw,avx512f")))
static float vec_dot_i8_avx512vnni(const void *a, const void *b, size_t bytes) {
    size_t n = bytes;
    const int8_t *ca = a, *cb = b;
    __m512i sum0 = _mm512_setzero_si512();
    __m512i sum1 = _mm512_setzero_si512();
    __m512i sum2 = _mm512_setzero_si512();
    __m512i sum3 = _mm512_setzero_si512();
    size_t i = 0;
    for (; i + 255 < n; i += 256) {
        sum0 = _mm512_dpbssd_epi32(sum0, _mm512_loadu_si512((const void *)(ca + i)), _mm512_loadu_si512((const void *)(cb + i)));
        sum1 = _mm512_dpbssd_epi32(sum1, _mm512_loadu_si512((const void *)(ca + i + 64)), _mm512_loadu_si512((const void *)(cb + i + 64)));
        sum2 = _mm512_dpbssd_epi32(sum2, _mm512_loadu_si512((const void *)(ca + i + 128)), _mm512_loadu_si512((const void *)(cb + i + 128)));
        sum3 = _mm512_dpbssd_epi32(sum3, _mm512_loadu_si512((const void *)(ca + i + 192)), _mm512_loadu_si512((const void *)(cb + i + 192)));
    }
    sum0 = _mm512_add_epi32(_mm512_add_epi32(sum0, sum1), _mm512_add_epi32(sum2, sum3));
    for (; i + 63 < n; i += 64) {
        sum0 = _mm512_dpbssd_epi32(sum0, _mm512_loadu_si512((const void *)(ca + i)), _mm512_loadu_si512((const void *)(cb + i)));
    }
    int32_t sum = _mm512_reduce_add_epi32(sum0);
    for (; i < n; i++) sum += (int32_t)ca[i] * (int32_t)cb[i];
    return (float)sum;
}

__attribute__((target("avx2")))
static float vec_dot_i8_avx2(const void *a, const void *b, size_t bytes) {
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

static float vec_dot_i8(const void *a, const void *b, size_t bytes) {
    if (__builtin_cpu_supports("avx512vnni")) {
        return vec_dot_i8_avx512vnni(a, b, bytes);
    }
    return vec_dot_i8_avx2(a, b, bytes);
}

__attribute__((target("popcnt")))
static float vec_dot_b8(const void *a, const void *b, size_t bytes) {
    size_t n = bytes / 8;
    const uint64_t *la = a, *lb = b;
    uint64_t pop = 0;
    for (size_t i = 0; i < n; i++) pop += __builtin_popcountll(la[i] ^ lb[i]);
    const uint8_t *ca = a, *cb = b;
    for (size_t i = n * 8; i < bytes; i++) pop += __builtin_popcount(ca[i] ^ cb[i]);
    return -(float)pop;
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
    else if (!strcmp(type, "b8")) dot_fn = vec_dot_b8;
    else diex("invalid type or length");

    float best_score = -1e30f;
    const char *best_k = NULL;
    uint16_t best_k_len = 0;

    uint64_t start_idx, end_idx;
    ht_tenant_range(t, r->t_len, &start_idx, &end_idx);

    uint64_t count = end_idx > start_idx ? end_idx - start_idx : 0;
    uint64_t *offs = get_sorted_offs(start_idx, count);

    #pragma omp parallel num_threads(worker_threads())
    {
        float local_best = -1e30f;
        const char *local_k = NULL;
        uint16_t local_k_len = 0;

        #pragma omp for schedule(static, 4096)
        for (uint64_t i = 0; i < count; i++) {
            uint64_t koff = offs ? offs[i] : ht[start_idx + i].off1 - 1;
            Record *c = rec_at(koff);
            if (c->op == OP_DEL || c->t_len != r->t_len || c->v_len != r->v_len) continue;
            if (koff + 1 == n->off1) continue; // skip self
            if (memcmp(rec_t(c), t, r->t_len) != 0) continue;

            size_t p1 = r->v_len > 256 ? 256 : r->v_len;
            float s1 = dot_fn(rec_v(r), rec_v(c), p1);
            if (p1 < r->v_len && s1 * ((float)r->v_len / p1) < local_best - 0.8f) continue;
            size_t p2 = r->v_len > 1024 ? 1024 : r->v_len;
            float s2 = p1 < p2 ? s1 + dot_fn(rec_v(r) + p1, rec_v(c) + p1, p2 - p1) : s1;
            if (p2 < r->v_len && s2 * ((float)r->v_len / p2) < local_best - 0.3f) continue;
            float score = p2 < r->v_len ? s2 + dot_fn(rec_v(r) + p2, rec_v(c) + p2, r->v_len - p2) : s2;
            if (score > local_best) {
                local_best = score;
                local_k = rec_k(c);
                local_k_len = c->k_len;
                if (score > *(volatile float *)&best_score) {
                    #pragma omp critical
                    {
                        if (score > best_score) { best_score = score; best_k = local_k; best_k_len = local_k_len; }
                    }
                }
            }
            if (*(volatile float *)&best_score > local_best) local_best = *(volatile float *)&best_score;
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

    if (offs) munmap(offs, count * 8);

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

struct rseq {
    uint32_t cpu_id_start;
    uint32_t cpu_id;
    uint64_t rseq_cs;
    uint32_t flags;
} __attribute__((aligned(32)));
extern __thread struct rseq __rseq_abi __attribute__((weak));

static inline int fast_getcpu(void) {
    if (&__rseq_abi && __rseq_abi.cpu_id < 1000000) return __rseq_abi.cpu_id_start;
    return sched_getcpu();
}

static int batch_flush(int fd, char *buf, size_t *used)
{
    if (!*used) {
        return 1;
    }

    struct statvfs st;
    if (!fstatvfs(fd, &st) && st.f_blocks > 0 && st.f_frsize > 0) {
        uint64_t free_bytes = (uint64_t)st.f_bavail * st.f_frsize;
        uint64_t reserve_bytes = (uint64_t)((long double)st.f_blocks * st.f_frsize * 0.1L);
        uint64_t need = *used + BATCH_WRITE_BYTES;
        if (free_bytes < reserve_bytes + need) {
            return 0;
        }
    }

    off_t pos = lseek(fd, 0, SEEK_END);
    if (pos >= 0) {
        off_t end = pos + (off_t)*used;
        (void)fallocate(fd, FALLOC_FL_KEEP_SIZE, 0, (end + 33554431) & ~(off_t)33554431);
    }

    write_all(fd, buf, *used);
    *used = 0;
    return 1;
}

static int batch_put(int fd, char *buf, size_t *used, const char *t, size_t tl, const char *k, size_t kl, const char *v, size_t vl)
{
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
    struct statvfs st;
    if (!*used && !fstatvfs(fd, &st) && st.f_blocks > 0 && st.f_frsize > 0) {
        uint64_t free_bytes = (uint64_t)st.f_bavail * st.f_frsize;
        uint64_t reserve_bytes = (uint64_t)((long double)st.f_blocks * st.f_frsize * 0.1L);
        if (free_bytes < reserve_bytes + r.len) return 0;
    }
    if (tl > 0 && t[0] != '0') {
        if (ht_cap) {
            Node *n = ht_get(t, r.t_len, k, r.k_len);
            if (n && rec_at(n->off1 - 1)->op != OP_DEL) {
                r.weight_log = rec_at(n->off1 - 1)->weight_log;
            }
        }
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
        if (!batch_put(fd, buf, &used, t, strlen(t), line, tab - line, tab + 1, strlen(tab + 1))) {
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
        ssize_t r = send(fd, p, n, MSG_NOSIGNAL);
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
        if (len > 32u * 1024u * 1024u) {
            status = 3;
            payload = "shed";
            n = 4;
            len = 16 + n;
        } else {
            resp_c = chunk_pop();
            if (!resp_c) return;
            buf = resp_c->data;
        }
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

typedef struct {
    Chunk *chunk;
    char pad[64 - sizeof(Chunk *)];
} __attribute__((aligned(64))) CacheLine;
static CacheLine cpu_chunks[1024];

static inline Chunk *chunk_swap(Chunk *in) {
    if (&__rseq_abi && __rseq_abi.cpu_id < 1000000) {
        uint64_t cs[4] __attribute__((aligned(32))) = {0};
        int ok = 1;
        Chunk *scratch;
        uint64_t vcpu;
        __asm__ __volatile__ (
            "leaq 90f(%%rip), %%rax\n\t"
            "movq %%rax, 8(%[cs])\n\t"
            "leaq 91f(%%rip), %%rax\n\t"
            "subq 8(%[cs]), %%rax\n\t"
            "movq %%rax, 16(%[cs])\n\t"
            "leaq 92f(%%rip), %%rax\n\t"
            "movq %%rax, 24(%[cs])\n\t"
            "movq %[cs], 8(%[rseq])\n\t"
            "90:\n\t"
            "movl 4(%[rseq]), %k[vcpu]\n\t"
            "andl $1023, %k[vcpu]\n\t"
            "shlq $6, %q[vcpu]\n\t"
            "addq %[ptrs], %q[vcpu]\n\t"
            "movq (%q[vcpu]), %[scratch]\n\t"
            "movq %[in], (%q[vcpu])\n\t"
            "91:\n\t"
            "jmp 93f\n\t"
            ".int 0x53053053\n\t"
            "92:\n\t"
            "xorl %[ok], %[ok]\n\t"
            "93:\n\t"
            "movq $0, 8(%[rseq])\n\t"
            : [ok] "+r" (ok), [scratch] "=&r" (scratch), [vcpu] "=&r" (vcpu)
            : [cs] "r" (cs), [rseq] "r" (&__rseq_abi),
              [ptrs] "r" (cpu_chunks), [in] "r" (in)
            : "rax", "memory", "cc"
        );
        if (ok) return scratch;
    }
    int cpu = sched_getcpu();
    if (cpu < 0 || cpu >= 1024) cpu = 0;
    return __atomic_exchange_n(&cpu_chunks[cpu].chunk, in, __ATOMIC_RELAXED);
}

static inline void chunk_push(Chunk *chunk) {
    if (!chunk) return;
    Chunk *old = chunk_swap(chunk);
    if (old) munmap(old, sizeof(*old));
}

static inline Chunk *chunk_pop(void) {
    Chunk *chunk = chunk_swap(NULL);
    if (chunk) return chunk;

    static _Atomic uint64_t last_check = 0;
    static _Atomic int last_res = 1;
    uint64_t now = time(NULL);
    int res = __atomic_load_n(&last_res, __ATOMIC_RELAXED);
    if (now != __atomic_load_n(&last_check, __ATOMIC_RELAXED)) {
        uint64_t total = 0;
        uint64_t freeish = get_mem_avail(&total);
        res = 1;
        if (total && freeish < total / 10 + sizeof(Chunk)) res = 0;
        double load[1];
        if (res && getloadavg(load, 1) == 1 && load[0] > omp_get_num_procs() * 0.9) res = 0;
        __atomic_store_n(&last_res, res, __ATOMIC_RELAXED);
        __atomic_store_n(&last_check, now, __ATOMIC_RELAXED);
    }
    if (!res) return NULL;
    chunk = mmap(NULL, sizeof(*chunk), PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    if (chunk == MAP_FAILED) return NULL;
    madvise(chunk, sizeof(*chunk), MADV_HUGEPAGE);
    return chunk;
}

static int srv_db_fd = -1;
#include <sys/resource.h>
#include <pthread.h>
static struct { pthread_rwlock_t rw; char pad[64 - sizeof(pthread_rwlock_t)]; } srv_rwlocks[256];
static pthread_mutex_t srv_all_lock = PTHREAD_MUTEX_INITIALIZER;
static int srv_rwlocks_init = 0;
static inline void srv_init_locks(void) {
    if (!__atomic_load_n(&srv_rwlocks_init, __ATOMIC_ACQUIRE)) {
        #pragma omp critical (init)
        if (!__atomic_load_n(&srv_rwlocks_init, __ATOMIC_ACQUIRE)) {
            for (int i = 0; i < 256; i++) pthread_rwlock_init(&srv_rwlocks[i].rw, NULL);
            __atomic_store_n(&srv_rwlocks_init, 1, __ATOMIC_RELEASE);
        }
    }
}
static inline void srv_lock_all(void) {
    pthread_mutex_lock(&srv_all_lock);
    for (int i = 0; i < 256; i++) pthread_rwlock_wrlock(&srv_rwlocks[i].rw);
}
static inline void srv_unlock_all(void) {
    for (int i = 256; i-- > 0; ) pthread_rwlock_unlock(&srv_rwlocks[i].rw);
    pthread_mutex_unlock(&srv_all_lock);
}
static inline int srv_read_lock_func(const char *db_path) {
    srv_init_locks();
    int cpu = fast_getcpu();
    if (cpu < 0 || cpu >= 256) cpu = 0;
    pthread_rwlock_rdlock(&srv_rwlocks[cpu].rw);
    struct stat st, fd_st;
    int needs_reopen = (srv_db_fd < 0) || (!stat(db_path, &st) && (!fstat(srv_db_fd, &fd_st) && st.st_ino != fd_st.st_ino || st.st_size > (off_t)map_size));
    if (needs_reopen || ht_len != ht_sorted_len) {
        pthread_rwlock_unlock(&srv_rwlocks[cpu].rw);
        srv_lock_all();
        needs_reopen = (srv_db_fd < 0) || (!stat(db_path, &st) && (!fstat(srv_db_fd, &fd_st) && st.st_ino != fd_st.st_ino || st.st_size > (off_t)map_size));
        if (needs_reopen) {
            if (srv_db_fd >= 0 && (!fstat(srv_db_fd, &fd_st) && st.st_ino != fd_st.st_ino)) { close(srv_db_fd); srv_db_fd = -1; }
            load_db(db_path);
            if (srv_db_fd < 0) srv_db_fd = open_append(db_path);
        }
        if (ht_len != ht_sorted_len) deduplicate_ht();
        srv_unlock_all();
        pthread_rwlock_rdlock(&srv_rwlocks[cpu].rw);
    }
    return cpu;
}
static inline int srv_write_lock_func(const char *db_path) {
    srv_init_locks();
    srv_lock_all();
    struct stat st, fd_st;
    if (srv_db_fd >= 0 && !stat(db_path, &st) && !fstat(srv_db_fd, &fd_st) && st.st_ino != fd_st.st_ino) {
        close(srv_db_fd); srv_db_fd = -1;
    }
    if (srv_db_fd < 0 || (!stat(db_path, &st) && st.st_size > (off_t)map_size)) {
        load_db(db_path);
        if (srv_db_fd < 0) srv_db_fd = open_append(db_path);
    }
    return 0;
}
static inline void srv_unlock_read_attr(int *cpu) {
    if (*cpu >= 0) pthread_rwlock_unlock(&srv_rwlocks[*cpu].rw);
}
static inline void srv_unlock_write_attr(int *dummy) {
    (void)dummy;
    srv_unlock_all();
}
#define SRV_READ_LOCK(path) int _srv_cpu __attribute__((cleanup(srv_unlock_read_attr))) = srv_read_lock_func(path)
#define SRV_WRITE_LOCK(path) int _srv_dummy __attribute__((cleanup(srv_unlock_write_attr))) = srv_write_lock_func(path)

static void do_serve(const char *db_path, int port, int32_t cipherkey) {
    omp_set_dynamic(1);
    omp_set_max_active_levels(2);

    struct rlimit rl;
    if (!getrlimit(RLIMIT_NOFILE, &rl)) {
        rl.rlim_cur = rl.rlim_max;
        setrlimit(RLIMIT_NOFILE, &rl);
    }
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
            #pragma omp task
            {
                while (1) {
                    usleep(1000000);
                    int need_compact = 0;
                    int fd_to_sync = srv_db_fd;
                    if (fd_to_sync >= 0) {
                        fdatasync(fd_to_sync);
                        struct statvfs st;
                        if (!fstatvfs(fd_to_sync, &st) && st.f_blocks > 0 && st.f_frsize > 0) {
                            uint64_t free_bytes = (uint64_t)st.f_bavail * st.f_frsize;
                            uint64_t reserve_bytes = (uint64_t)((long double)st.f_blocks * st.f_frsize * 0.15L);
                            if (free_bytes < reserve_bytes) need_compact = 1;
                        }
                    }
                    if (need_compact) { SRV_WRITE_LOCK(db_path); do_compact(db_path); if (srv_db_fd >= 0) { close(srv_db_fd); srv_db_fd = -1; } }
                }
            }
            while (1) {
                int fd = accept(srv, NULL, NULL);
                if (fd >= 0) {
                    if (__atomic_fetch_add(&active_conn, 1, __ATOMIC_RELAXED) >= max_conn) {
                        __atomic_fetch_sub(&active_conn, 1, __ATOMIC_RELAXED);
                        close(fd);
                        continue;
                    }
                    setsockopt(fd, IPPROTO_TCP, TCP_NODELAY, &opt, sizeof(opt));
#ifdef TCP_QUICKACK
                    setsockopt(fd, IPPROTO_TCP, TCP_QUICKACK, &opt, sizeof(opt));
#endif
                    struct timeval tv = {30, 0};
                    setsockopt(fd, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));
                    setsockopt(fd, SOL_SOCKET, SO_SNDTIMEO, &tv, sizeof(tv));
                    #pragma omp task
                    {
                        while (1) {
                            uint32_t len;
                            if (!read_full(fd, &len, 4)) break;
                            len = ntohl(len);
                            if (len > 64 * 1024 * 1024) break;

                            uint8_t small_req[65536];
                            Chunk *c = NULL;
                            uint8_t *buf = small_req;
                            if (len > sizeof(small_req)) {
                                c = chunk_pop();
                                if (!c) break;
                                if (len > sizeof(c->data)) { chunk_push(c); break; }
                                buf = c->data;
                            }

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
                            { SRV_READ_LOCK(db_path);
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
                                { SRV_READ_LOCK(db_path);
                                    Node *n = ht_get(full_tenant, ft_len, k, kl);
                                    if (n) {
                                        Record *r = rec_at(n->off1 - 1);
                                        if (r->op != OP_DEL) send_response(fd, cipherkey, 0, rec_v(r), r->v_len);
                                        else send_response(fd, cipherkey, 2, "not found", 9);
                                    } else send_response(fd, cipherkey, 2, "not found", 9);
                                }
                            }
                            else if (op == 2 || op == 3) {
                                if (op == 2 && !(perms & 2)) { send_response(fd, cipherkey, 1, "denied", 6); chunk_push(c); continue; }
                                if (op == 3 && !(perms & 4)) { send_response(fd, cipherkey, 1, "denied", 6); chunk_push(c); continue; }
                                int wrote = 0;
                                int shed = 0;
                                if (op == 2) {
                                    { SRV_WRITE_LOCK(db_path);
                                        wrote = append_raw(srv_db_fd, full_tenant, ft_len, k, kl, v, vl, OP_PUT);
                                        (void)wrote;
                                    }
                                    if (wrote) send_response(fd, cipherkey, 0, v, vl);
                                    else send_response(fd, cipherkey, 3, "shed", 4);
                                } else if (vl == 1 && v[0] == '*') {
                                    if (kl < 3) { send_response(fd, cipherkey, 1, "prefix too short", 16); chunk_push(c); continue; }
                                    { SRV_WRITE_LOCK(db_path);
                                        uint64_t start_idx, end_idx;
                                        ht_tenant_range(full_tenant, ft_len, &start_idx, &end_idx);
                                        uint64_t count = end_idx > start_idx ? end_idx - start_idx : 0;
                                        uint64_t *offs = get_sorted_offs(start_idx, count);
                                        for (uint64_t i = 0; i < count; i++) {
                                            Record *r = rec_at(offs ? offs[i] : (ht[start_idx + i].off1 - 1));
                                            if (r->op == OP_DEL || r->t_len != ft_len || memcmp(rec_t(r), full_tenant, ft_len)) continue;
                                            if (kl && (r->k_len < kl || memcmp(rec_k(r), k, kl))) continue;
                                            if (!append_raw(srv_db_fd, full_tenant, ft_len, rec_k(r), r->k_len, NULL, 0, OP_DEL)) { shed = 1; break; }
                                            wrote++;
                                        }
                                        if (offs) munmap(offs, count * 8);
                                        if (wrote && !shed) load_db(db_path);
                                    }
                                    char res[64];
                                    int rl = snprintf(res, sizeof(res), "%d", wrote);
                                    send_response(fd, cipherkey, shed ? 3 : 0, shed ? "shed" : res, shed ? 4 : rl);
                                } else {
                                    { SRV_WRITE_LOCK(db_path);
                                        wrote = append_raw(srv_db_fd, full_tenant, ft_len, k, kl, NULL, 0, OP_DEL);
                                        (void)wrote;
                                    }
                                    send_response(fd, cipherkey, wrote ? 0 : 3, wrote ? "ok" : "shed", wrote ? 2 : 4);
                                }
                            }
                            else if (op == 4 || op == 5 || op == 8 || op == 9) {
                                if (!(perms & 1)) { send_response(fd, cipherkey, 1, "denied", 6); chunk_push(c); continue; }
                                Chunk *out_c = NULL;
                                uint8_t *out = NULL; size_t out_len = 0;
                                if (op == 4 || op == 5) {
                                    out_c = chunk_pop();
                                    if (!out_c) { send_response(fd, cipherkey, 3, "shed", 4); chunk_push(c); continue; }
                                    out = out_c->data;
                                }
                                #define OUT_CAP (60u * 1024u * 1024u)
                                uint64_t query_bfs[64] = {0};
                                int query_inv[64] = {0};
                                int num_words = 0; char *w = v; char *end = v + vl;
                                while (w < end && num_words < 64) {
                                    char *tab = memchr(w, '\t', end - w);
                                    size_t wl = tab ? tab - w : end - w;
                                    if (wl > 0) {
                                        int inv = w[0] == '-';
                                        query_inv[num_words] = inv;
                                        if (wl > inv) query_bfs[num_words] = compute_bf(w + inv, wl - inv);
                                        num_words++;
                                    }
                                    w += wl + 1;
                                }

                                double min_weight = 0.5;
                                double threshold = 1.0; double eval_now = (double)time(NULL);
                                char *pref = k; size_t pref_len = kl;
                                if (kl > 0) {
                                    char *tab3 = memrchr(k, '\t', kl);
                                    if (tab3) {
                                        char *tab2 = memrchr(k, '\t', tab3 - k);
                                        if (tab2) {
                                            char *tab1 = memrchr(k, '\t', tab2 - k);
                                            if (tab1) {
                                                *tab1 = *tab2 = *tab3 = 0;
                                                pref_len = tab1 - k;
                                                threshold = strtod(tab1 + 1, NULL);
                                                eval_now = strtod(tab2 + 1, NULL);
                                                min_weight = strtod(tab3 + 1, NULL);
                                            } else {
                                                *tab2 = *tab3 = 0;
                                                pref_len = tab2 - k;
                                                threshold = strtod(tab2 + 1, NULL);
                                                eval_now = strtod(tab3 + 1, NULL);
                                            }
                                        }
                                    }
                                }
{ double _sn = (double)time(NULL); if (eval_now < 1 || eval_now > _sn) eval_now = _sn; }
double max_w = threshold > 1.0 ? threshold : 13.0;

                                double count_est = 0; uint64_t raw_count = 0; double sum = 0; double raw_sum = 0;

                                { SRV_READ_LOCK(db_path);
                                uint64_t start_idx, end_idx;
                                ht_tenant_range(full_tenant, ft_len, &start_idx, &end_idx);
                                uint64_t count = end_idx > start_idx ? end_idx - start_idx : 0;
                                uint64_t *offs = get_sorted_offs(start_idx, count);
                                #pragma omp parallel for reduction(+:count_est,raw_count,sum,raw_sum) schedule(static, 4096) num_threads(worker_threads())
                                for (uint64_t i = 0; i < count; i++) {
                                    Record *r = rec_at(offs ? offs[i] : (ht[start_idx + i].off1 - 1));
                                    if (r->op == OP_DEL || r->t_len != ft_len || memcmp(rec_t(r), full_tenant, ft_len)) continue;
                                    if (pref_len > 0 && (r->k_len < pref_len || memcmp(rec_k(r), pref, pref_len))) continue;
                                    if (r->weight_log > max_w) continue;

                                    const char *out_val = rec_v(r);
                                    size_t out_vl = r->v_len;
                                    char eval_buf[64];
                                    double cur = 0;
                                    int is_decay = 0;
                                    if (out_vl > 0 && out_vl < 192) {
                                        if (decay_value_at(out_val, out_vl, eval_now, &cur)) {
                                            if (cur == 0) continue;
                                            out_vl = snprintf(eval_buf, sizeof(eval_buf), "%.17g", cur);
                                            out_val = eval_buf;
                                            is_decay = 1;
                                        }
                                    }

                                    int match = 1;
                                    char *w_iter = v;
                                    int word_idx = 0;
                                    while (w_iter < end && match) {
                                        char *tab = memchr(w_iter, '\t', end - w_iter);
                                        size_t wl = tab ? tab - w_iter : end - w_iter;
                                        if (wl > 0) {
                                            int inv = word_idx < 64 ? query_inv[word_idx] : (w_iter[0] == '-');
                                            char *kw = w_iter + inv;
                                            size_t kwl = wl - inv;
                                            if (kwl > 0) {
                                                int bf_match = word_idx < 64 ? ((r->bf & query_bfs[word_idx]) == query_bfs[word_idx]) : ((r->bf & compute_bf(kw, kwl)) == compute_bf(kw, kwl));
                                                int found = 0;
                                                if (bf_match) found = (!is_decay && memmem_pivot(out_val, out_vl, kw, kwl)) || memmem_pivot(rec_k(r), r->k_len, kw, kwl);
                                                if (inv ? found : !found) match = 0;
                                            }
                                            word_idx++;
                                        }
                                        w_iter += wl + 1;
                                    }
                                    if (match) {
                                        double db_w = (double)(1U << (r->weight_log > 13 ? 13 : r->weight_log));
                                        double w_weight = db_w;
                                        double eff_w = db_w;
                                        double disp_w = is_decay ? db_w * __builtin_fabs(cur) : db_w;
                                        if (op == 4 || op == 5) { if (disp_w < min_weight) continue;
                                            char weight[32];
                                            int wlen = snprintf(weight, sizeof(weight), "%.5g\t", disp_w);
                                            size_t rec_len = wlen + r->k_len + 1 + out_vl + 1;
                                            size_t my_off;
                                            #pragma omp atomic capture
                                            { my_off = out_len; out_len += rec_len; }
                                            if (my_off + rec_len <= OUT_CAP) {
                                                memcpy(out + my_off, weight, wlen); my_off += wlen;
                                                memcpy(out + my_off, rec_k(r), r->k_len); my_off += r->k_len;
                                                out[my_off++] = '\t';
                                                memcpy(out + my_off, out_val, out_vl); my_off += out_vl;
                                                out[my_off++] = '\n';
                                            }
                                        } else {
                                    if (disp_w < min_weight) continue;
                                    count_est += eff_w;
                                    raw_count++;
                                            if (op == 9 && r->v_len > 0) {
                                                if (is_decay) {
                                                    sum += w_weight * cur;
                                                    raw_sum += cur;
                                                } else if (r->v_len < 192) {
                                                    char buf2[192] = {0};
                                                    memcpy(buf2, rec_v(r), r->v_len);
                                    double v = strtod(buf2, NULL);
                                    sum += v * w_weight;
                                    raw_sum += v;
                                                }
                                            }
                                        }
                                    }
                                }
                                if (offs) munmap(offs, count * 8);
                                }
                                if (op == 4 || op == 5) {
                                    send_response(fd, cipherkey, out_len > OUT_CAP ? 4 : 0, out, out_len > OUT_CAP ? OUT_CAP : out_len);
                                    if (out_c) chunk_push(out_c);
                                } else {
                                    char val[128]; int vl_out = 0;
                                    if (raw_count < 30) { count_est = raw_count; sum = raw_sum; }
                                    if (op == 8) vl_out = snprintf(val, sizeof(val), "%.4g\t%llu", count_est, (unsigned long long)raw_count);
                                    else vl_out = snprintf(val, sizeof(val), "%.17g\t%.17g\t%llu", sum, raw_sum, (unsigned long long)raw_count);
                                    send_response(fd, cipherkey, 0, val, vl_out);
                                }
                            } else if (op == 10) {
                                if (!(perms & 2)) { send_response(fd, cipherkey, 1, "denied", 6); chunk_push(c); continue; }
                                char val[64]; int vl_out = 0; int wrote = 0;
                                { SRV_WRITE_LOCK(db_path);
                                    long long cur = 0;
                                    Node *n = ht_get(full_tenant, ft_len, k, kl);
                                    if (n) { Record *r = rec_at(n->off1 - 1); if (r->op != OP_DEL && r->v_len < 64) { char buf[64]={0}; memcpy(buf, rec_v(r), r->v_len); cur = strtoll(buf, NULL, 10); } }
                                    char d_buf[64]={0}; memcpy(d_buf, v, vl < 63 ? vl : 63);
                                    long long d = strtoll(d_buf, NULL, 10);
                                    long long next = cur + d;
                                    vl_out = snprintf(val, sizeof(val), "%lld", next);
                                    wrote = append_raw(srv_db_fd, full_tenant, ft_len, k, kl, val, vl_out, OP_PUT);
(void)wrote;
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
                                { SRV_WRITE_LOCK(db_path);
                                    wrote = append_raw(srv_db_fd, "0:users", 7, user_key, user_key_len, user_val, user_val_len, OP_PUT);
(void)wrote;
                                }
                                if (wrote) send_response(fd, cipherkey, 0, "ok", 2);
                                else send_response(fd, cipherkey, 3, "shed", 4);
                            } else if (op == 12) {
                                if (!(perms & 2)) { send_response(fd, cipherkey, 1, "denied", 6); chunk_push(c); continue; }
                                char val[64]; int vl_out = 0; int ok = 0; int shed = 0;
                                { SRV_WRITE_LOCK(db_path);
                                    long long cur = 0;
                                    Node *n = ht_get(full_tenant, ft_len, k, kl);
                                    if (n) { Record *r = rec_at(n->off1 - 1); if (r->op != OP_DEL && r->v_len < 64) { char buf[64]={0}; memcpy(buf, rec_v(r), r->v_len); cur = strtoll(buf, NULL, 10); } }
                                    if (cur > 0) {
                                        vl_out = snprintf(val, sizeof(val), "%lld", cur - 1);
                                        ok = append_raw(srv_db_fd, full_tenant, ft_len, k, kl, val, vl_out, OP_PUT);
                                        if (ok) {
                                            (void)0;
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
                                { SRV_WRITE_LOCK(db_path);
                                    Node *n = ht_get(full_tenant, ft_len, k, kl);
                                    if (n) { Record *r = rec_at(n->off1 - 1); if (r->op != OP_DEL) exists = 1; }
                                    if (!exists) {
                                        wrote = append_raw(srv_db_fd, full_tenant, ft_len, k, kl, v, vl, OP_PUT);
(void)wrote;
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
                                { SRV_WRITE_LOCK(db_path);
                                    Node *n = ht_get(full_tenant, ft_len, k, kl);
                                    if (n) {
                                        Record *r = rec_at(n->off1 - 1);
                                        if (r->op != OP_DEL && r->v_len == old_len && !memcmp(rec_v(r), v, old_len)) match = 1;
                                    }
                                    if (match) {
                                        wrote = append_raw(srv_db_fd, full_tenant, ft_len, k, kl, new_val, new_len, OP_PUT);
(void)wrote;
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
                                char *e1;
                                char *e2;
                                char *e3;
                                errno = 0;
                                double hl = strtod(v_null, &e1);
                                double ts = strtod(tab1 + 1, &e2);
                                double d = strtod(tab2 + 1, &e3);
                                if (errno || e1 != tab1 || e2 != tab2 || *e3 || hl <= 0 || !__builtin_isfinite(hl) || !__builtin_isfinite(ts) || !__builtin_isfinite(d)) {
                                    send_response(fd, cipherkey, 1, "bad arg", 7); chunk_push(c); continue;
                                }

                                char val[192]; int vl_out = 0; int ok = 1; int shed = 0; char err_msg[32] = "error";
                                { SRV_WRITE_LOCK(db_path);
                                    double stored_hl = hl, last = ts, value = 0;
                                    int had = 0;
                                    Node *n = ht_get(full_tenant, ft_len, k, kl);
                                    if (n) {
                                        Record *r = rec_at(n->off1 - 1);
                                        if (r->op != OP_DEL && r->v_len > 0 && r->v_len < 192 && rec_v(r)[0] == '\x1F') {
                                            char buf[192] = {0}; memcpy(buf, rec_v(r) + 1, r->v_len - 1);
                                            char *t1 = strchr(buf, '\t'); char *t2 = t1 ? strchr(t1 + 1, '\t') : NULL;
                                            if (t1 && t2) {
                                                *t1 = 0; *t2 = 0;
                                                stored_hl = strtod(buf, NULL); last = strtod(t1 + 1, NULL); value = strtod(t2 + 1, NULL);
                                                had = 1;
                                            }
                                        }
                                    }
                                    if (had && stored_hl != hl) { ok = 0; strcpy(err_msg, "hl changed"); }
                                    else {
                                        double next;
                                        if (had && ts < last) {
                                            next = value + d * __builtin_exp2((ts - last) / hl);
                                            ts = last;
                                        } else {
                                            next = value * __builtin_exp2((last - ts) / hl) + d;
                                        }
                                        if (!__builtin_isfinite(next)) { ok = 0; strcpy(err_msg, "not finite"); }
                                        else if (__builtin_fabs(next) < 1e-12) {
                                            if (had) {
                                                ok = append_raw(srv_db_fd, full_tenant, ft_len, k, kl, NULL, 0, OP_DEL);
                                                if (ok) (void)0;
                                                else shed = 1;
                                            }
                                            strcpy(val, "0"); vl_out = 1;
                                        } else {
                                            vl_out = snprintf(val, sizeof(val), "\x1F%.17g\t%.17g\t%.17g", hl, ts, next);
                                            ok = append_raw(srv_db_fd, full_tenant, ft_len, k, kl, val, vl_out, OP_PUT);
                                            if (ok) (void)0;
                                            else shed = 1;
                                        }
                                        if (ok) load_db(db_path);
                                    }
                                }
                                if (ok) send_response(fd, cipherkey, 0, val, vl_out);
                                else if (shed) send_response(fd, cipherkey, 3, "shed", 4);
                                else send_response(fd, cipherkey, 2, err_msg, strlen(err_msg));
                            } else if (op == 17) {
                                if ((perms & 5) != 5) { send_response(fd, cipherkey, 1, "denied", 6); chunk_push(c); continue; }
                                int wrote = 0;
                                char *ret_k = NULL;
                                uint16_t ret_kl = 0;
                                uint32_t ret_vl = 0;
                                uint64_t found_off1 = 0;
                                { SRV_READ_LOCK(db_path);
                                    uint64_t start_idx, end_idx;
                                    ht_tenant_range(full_tenant, ft_len, &start_idx, &end_idx);
                                    if (end_idx > start_idx) {
                                        uint64_t count = end_idx > start_idx ? end_idx - start_idx : 0;
                                        uint64_t *offs = get_sorted_offs(start_idx, count);
                                        static _Atomic uint64_t seed = 0x12345678;
                                        uint64_t s = __atomic_fetch_add(&seed, 6364136223846793005ULL, __ATOMIC_RELAXED);
                                        uint64_t start_offset = s % count;
                                        for (uint64_t i = 0; i < count; i++) {
                                            uint64_t idx = (start_offset + i) % count;
                                            Record *r = rec_at(offs ? offs[idx] : (ht[start_idx + idx].off1 - 1));
                                            if (r->op == OP_DEL || r->t_len != ft_len || memcmp(rec_t(r), full_tenant, ft_len) || r->weight_log > 0) continue;
                                            if (kl && (r->k_len < kl || memcmp(rec_k(r), k, kl))) continue;
                                            found_off1 = (offs ? offs[idx] : ht[start_idx + idx].off1 - 1) + 1;
                                            break;
                                        }
                                        if (offs) munmap(offs, count * 8);
                                    }
                                }
                                if (found_off1) {
                                    SRV_WRITE_LOCK(db_path);
                                    Record *r = rec_at(found_off1 - 1);
                                    if (r->op != OP_DEL && r->t_len == ft_len && !memcmp(rec_t(r), full_tenant, ft_len)) {
                                        Chunk *out_c = chunk_pop();
                                        if (out_c) {
                                            ret_k = (char *)out_c->data;
                                            ret_kl = r->k_len;
                                            ret_vl = r->v_len;
                                            if (ret_kl + 1 + ret_vl <= sizeof(out_c->data)) {
                                                memcpy(ret_k, rec_k(r), ret_kl);
                                                ret_k[ret_kl] = '\t';
                                                memcpy(ret_k + ret_kl + 1, rec_v(r), ret_vl);
                                                if (vl > 0) {
                                                    wrote = append_raw(srv_db_fd, full_tenant, ft_len, rec_k(r), r->k_len, v, vl, OP_PUT);
                                                } else {
                                                    wrote = append_raw(srv_db_fd, full_tenant, ft_len, rec_k(r), r->k_len, NULL, 0, OP_DEL);
                                                }
(void)wrote;
                                            } else {
                                                chunk_push(out_c);
                                                ret_k = NULL;
                                            }
                                        }
                                    }
                                }
                                if (wrote) {
                                    send_response(fd, cipherkey, 0, ret_k, ret_kl + 1 + ret_vl);
                                    chunk_push((Chunk *)ret_k);
                                } else if (ret_k) {
                                    send_response(fd, cipherkey, 3, "shed", 4);
                                    chunk_push((Chunk *)ret_k);
                                } else {
                                    send_response(fd, cipherkey, 2, "empty", 5);
                                }
                            } else if (op == 6) {
                                if (!(perms & 1)) { send_response(fd, cipherkey, 1, "denied", 6); chunk_push(c); continue; }
                                char off_str[64] = {0};
                                memcpy(off_str, k, kl < 63 ? kl : 63);
                                uint64_t off = strtoull(off_str, NULL, 10);
                                Chunk *out_c = chunk_pop();
                                if (!out_c) { send_response(fd, cipherkey, 3, "shed", 4); chunk_push(c); continue; }
                                size_t used = 0;
                                { SRV_READ_LOCK(db_path);
                                    while (off < valid_size) {
                                        if (!rec_valid(off)) break;
                                        Record *r = rec_at(off);
                                        uint64_t next = off + r->len;
                                        if (r->t_len == ft_len && !memcmp(rec_t(r), full_tenant, ft_len)) {
                                            if (used + 256 + r->k_len + r->v_len > sizeof(out_c->data)) break;
                                            int wl = snprintf((char*)out_c->data + used, 128, "%llu\t%llu\t%s\t%u\t", (unsigned long long)next, (unsigned long long)off, r->op == OP_PUT ? "put" : "del", 1U << r->weight_log);
                                            used += wl;
                                            memcpy(out_c->data + used, rec_k(r), r->k_len); used += r->k_len;
                                            out_c->data[used++] = '\t';
                                            if (r->op == OP_PUT) { memcpy(out_c->data + used, rec_v(r), r->v_len); used += r->v_len; }
                                            out_c->data[used++] = '\n';
                                        }
                                        off = next;
                                    }
                                }
                                send_response(fd, cipherkey, 0, out_c->data, used);
                                chunk_push(out_c);
                            } else if (op == 7) {
                                if (!(perms & 1)) { send_response(fd, cipherkey, 1, "denied", 6); chunk_push(c); continue; }
                                char type_buf[16] = {0};
                                memcpy(type_buf, v, vl < 15 ? vl : 15);
                                float (*dot_fn)(const void *, const void *, size_t) = NULL;
                                float best_score = -1e30f;
                                char best_k_buf[65536];
                                uint16_t best_k_len = 0;
                                int found = 0;
                                { SRV_READ_LOCK(db_path);
                                    Node *n = ht_get(full_tenant, ft_len, k, kl);
                                    if (n) {
                                        Record *r = rec_at(n->off1 - 1);
                                        if (r->op != OP_DEL && r->v_len > 0) {
                                            if (!strcmp(type_buf, "f32") && r->v_len % 4 == 0) dot_fn = vec_dot_f32;
                                            else if (!strcmp(type_buf, "f16") && r->v_len % 2 == 0) dot_fn = vec_dot_f16;
                                            else if (!strcmp(type_buf, "i8")) dot_fn = vec_dot_i8;
                                            else if (!strcmp(type_buf, "b8")) dot_fn = vec_dot_b8;
                                            if (dot_fn) {
                                                uint64_t start_idx, end_idx;
                                                ht_tenant_range(full_tenant, ft_len, &start_idx, &end_idx);
                                                uint64_t count = end_idx > start_idx ? end_idx - start_idx : 0;
                                                uint64_t *offs = get_sorted_offs(start_idx, count);
                                                #pragma omp parallel num_threads(worker_threads())
                                                {
                                                    float local_best = -1e30f;
                                                    const char *local_k = NULL;
                                                    uint16_t local_k_len = 0;
                                                    #pragma omp for schedule(static, 4096)
                                                    for (uint64_t i = 0; i < count; i++) {
                                                        uint64_t koff = offs ? offs[i] : ht[start_idx + i].off1 - 1;
                                                        Record *c_rec = rec_at(koff);
                                                        if (c_rec->op == OP_DEL || c_rec->t_len != r->t_len || c_rec->v_len != r->v_len) continue;
                                                        if (koff + 1 == n->off1) continue;
                                                        if (memcmp(rec_t(c_rec), full_tenant, r->t_len) != 0) continue;
                                                        size_t p1 = r->v_len > 256 ? 256 : r->v_len;
                                                        float s1 = dot_fn(rec_v(r), rec_v(c_rec), p1);
                                                        if (p1 < r->v_len && s1 * ((float)r->v_len / p1) < local_best - 0.8f) continue;
                                                        size_t p2 = r->v_len > 1024 ? 1024 : r->v_len;
                                                        float s2 = p1 < p2 ? s1 + dot_fn(rec_v(r) + p1, rec_v(c_rec) + p1, p2 - p1) : s1;
                                                        if (p2 < r->v_len && s2 * ((float)r->v_len / p2) < local_best - 0.3f) continue;
                                                        float score = p2 < r->v_len ? s2 + dot_fn(rec_v(r) + p2, rec_v(c_rec) + p2, r->v_len - p2) : s2;
                                                        if (score > local_best) {
                                                            local_best = score; local_k = rec_k(c_rec); local_k_len = c_rec->k_len;
                                                            if (score > *(volatile float *)&best_score) {
                                                                #pragma omp critical
                                                                { if (score > best_score) { best_score = score; memcpy(best_k_buf, local_k, local_k_len); best_k_len = local_k_len; found = 1; } }
                                                            }
                                                        }
                                                        if (*(volatile float *)&best_score > local_best) local_best = *(volatile float *)&best_score;
                                                    }
                                                    #pragma omp critical
                                                    { if (local_best > best_score && local_k) { best_score = local_best; memcpy(best_k_buf, local_k, local_k_len); best_k_len = local_k_len; found = 1; } }
                                                }
                                                if (offs) munmap(offs, count * 8);
                                            }
                                        }
                                    }
                                }
                                if (found) send_response(fd, cipherkey, 0, best_k_buf, best_k_len);
                                else send_response(fd, cipherkey, 2, "not found", 9);
                            } else if (op == 16) {
                                if (!(perms & 2)) { send_response(fd, cipherkey, 1, "denied", 6); chunk_push(c); continue; }
                                int wrote = 0;
                                int shed = 0;
                                int bad = 0;
                                char *end = v + vl;
                                for (char *p = v; p < end && !bad; ) {
                                    if (end - p < 6) { bad = 1; break; }
                                    uint16_t item_kl = ntohs(*(uint16_t*)p);
                                    uint32_t item_vl = ntohl(*(uint32_t*)(p + 2));
                                    p += 6;
                                    if (item_kl > UINT16_MAX || item_vl > UINT32_MAX || (size_t)(end - p) < (size_t)item_kl + item_vl) { bad = 1; break; }
                                    p += item_kl + item_vl;
                                }
                                { SRV_WRITE_LOCK(db_path); Chunk *batch_c = NULL;
                                    char *batch_buf = NULL;
                                    size_t used = 0;
                                    if (!bad) {

                                        batch_c = chunk_pop();
                                        if (!batch_c) {
                                            shed = 1;
                                        } else {
                                            batch_buf = (char *)batch_c->data;
                                        }
                                        for (char *p = v; p < end && !shed; ) {
                                            uint16_t item_kl = ntohs(*(uint16_t*)p);
                                            uint32_t item_vl = ntohl(*(uint32_t*)(p + 2));
                                            p += 6;
                                            uint8_t twl = 0;
                                            if (ft_len > 0 && full_tenant[0] != '0' && ht_cap) {
                                                Node *n = ht_get(full_tenant, ft_len, p, item_kl);
                                                if (n && rec_at(n->off1 - 1)->op != OP_DEL) twl = rec_at(n->off1 - 1)->weight_log;
                                            }
                                            Record r = {0};
                                            r.magic = MAGIC;
                                            r.len = (uint32_t)(sizeof(r) + ft_len + item_kl + item_vl);
                                            r.t_len = (uint8_t)ft_len;
                                            r.k_len = (uint16_t)item_kl;
                                            r.v_len = (uint32_t)item_vl;
                                            r.op = OP_PUT;
                                            r.weight_log = twl;
                                            r.bf = compute_bf(p, item_kl) | compute_bf(p + item_kl, item_vl);
                                            r.key_hash = key_hash(full_tenant, ft_len, p, item_kl);
                                            r.check = rec_check(&r, full_tenant, p, p + item_kl);
                                            if (used + r.len > sizeof(batch_c->data)) { shed = 1; break; }
                                            memcpy(batch_buf + used, &r, sizeof(r)); used += sizeof(r);
                                            memcpy(batch_buf + used, full_tenant, ft_len); used += ft_len;
                                            memcpy(batch_buf + used, p, item_kl); used += item_kl;
                                            memcpy(batch_buf + used, p + item_kl, item_vl); used += item_vl;
                                            p += item_kl + item_vl;
                                            wrote++;
                                        }
                                        if (batch_c) {
                                            if (!shed && used) {
                                                struct statvfs st;
                                                if (!fstatvfs(srv_db_fd, &st) && st.f_blocks > 0 && st.f_frsize > 0) {
                                                    if ((uint64_t)st.f_bavail * st.f_frsize < (uint64_t)((long double)st.f_blocks * st.f_frsize * 0.1L) + used) shed = 1;
                                                }
                                                if (!shed) {
                                                    off_t pos = lseek(srv_db_fd, 0, SEEK_END);
                                                    if (pos >= 0) (void)fallocate(srv_db_fd, FALLOC_FL_KEEP_SIZE, 0, (pos + used + 33554431) & ~(off_t)33554431);
                                                    write_all(srv_db_fd, batch_buf, used);
                                                }
                                            }
                                            chunk_push(batch_c);
                                        }
                                        if (wrote && !shed) (void)0;
                                        load_db(db_path);
                                    }
                                }
                                char res[64];
                                int status = 0;
                                int rl = snprintf(res, sizeof(res), "%d", wrote);
                                if (bad) {
                                    status = 1;
                                    rl = snprintf(res, sizeof(res), "bad batch");
                                } else if (shed) {
                                    status = 3;
                                    rl = snprintf(res, sizeof(res), "shed");
                                }
                                send_response(fd, cipherkey, status, res, rl);
                            } else {
                                send_response(fd, cipherkey, 1, "bad op", 6);
                            }
                            chunk_push(c);
                        }
                        close(fd);
                        __atomic_fetch_sub(&active_conn, 1, __ATOMIC_RELAXED);
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
            else if (!strcmp(args[0], "search") && n >= 3) do_search(args[1], n - 2, args + 2);
            else if (!strcmp(args[0], "closest") && n == 4) do_closest(db, args[1], args[2], args[3]);
            else if (!strcmp(args[0], "count") && n >= 2) {
                double count_est = 0; uint64_t raw_c = 0;
                size_t tl = strlen(args[1]);
                size_t pl = n >= 3 ? strlen(args[2]) : 0;
                double threshold = n >= 4 ? strtod(args[3], NULL) : 1.0;
double max_w = threshold > 1.0 ? threshold : 13.0;
if (threshold > 1.0) threshold = 1.0;
                double now = (double)time(NULL);
            uint64_t start_idx, end_idx; ht_tenant_range(args[1], tl, &start_idx, &end_idx);
            uint64_t count = end_idx > start_idx ? end_idx - start_idx : 0;
            uint64_t *offs = get_sorted_offs(start_idx, count);
            #pragma omp parallel for reduction(+:count_est,raw_c) schedule(static, 4096) num_threads(worker_threads())
            for (uint64_t i = 0; i < count; i++) {
                Record *r = rec_at(offs ? offs[i] : (ht[start_idx + i].off1 - 1));
                if (r->op == OP_DEL || r->t_len != tl || memcmp(rec_t(r), args[1], tl)) continue;
                    if (pl && (r->k_len < pl || memcmp(rec_k(r), args[2], pl))) continue;
                if (threshold >= 1.0 && r->weight_log > max_w) continue;
            if (threshold < 1.0 && r->weight_log == 0) {
                if ((double)(r->key_hash & 0xFFFFFFFFULL) * 0x1.0p-32 > threshold) continue;
            }
            double db_w = (double)(1U << (r->weight_log > 13 ? 13 : r->weight_log));

            double w = (threshold < 1.0 && r->weight_log == 0) ? db_w / threshold : db_w;
            if (r->v_len > 0 && r->v_len < 192) {
                double cur = 0;
                if (decay_value_at(rec_v(r), r->v_len, now, &cur) && cur == 0) continue;
            }
            count_est += w;
            raw_c++;
                }
                if (offs) munmap(offs, count * 8);
                if (raw_c < 30) count_est = raw_c;
                printf("%.4g\t%llu\n", count_est, (unsigned long long)raw_c);
            }
            else if (!strcmp(args[0], "sum") && n >= 2) {
                double s = 0; uint64_t raw_s = 0; double raw_sum = 0;
                size_t tl = strlen(args[1]);
                size_t pl = n >= 3 ? strlen(args[2]) : 0;
                double threshold = n >= 4 ? strtod(args[3], NULL) : 1.0;
double max_w = threshold > 1.0 ? threshold : 13.0;
if (threshold > 1.0) threshold = 1.0;
                double sum_now = n >= 5 ? strtod(args[4], NULL) : (double)time(NULL);
            uint64_t start_idx, end_idx; ht_tenant_range(args[1], tl, &start_idx, &end_idx);
            uint64_t count = end_idx > start_idx ? end_idx - start_idx : 0;
            uint64_t *offs = get_sorted_offs(start_idx, count);
            #pragma omp parallel for reduction(+:s,raw_s,raw_sum) schedule(static, 4096) num_threads(worker_threads())
            for (uint64_t i = 0; i < count; i++) {
                Record *r = rec_at(offs ? offs[i] : (ht[start_idx + i].off1 - 1));
                if (r->op == OP_DEL || r->t_len != tl || memcmp(rec_t(r), args[1], tl)) continue;
                    if (pl && (r->k_len < pl || memcmp(rec_k(r), args[2], pl))) continue;
                if (threshold >= 1.0 && r->weight_log > max_w) continue;
            if (threshold < 1.0 && r->weight_log == 0) {
                if ((double)(r->key_hash & 0xFFFFFFFFULL) * 0x1.0p-32 > threshold) continue;
            }
            double db_w = (double)(1U << (r->weight_log > 13 ? 13 : r->weight_log));

            double w = (threshold < 1.0 && r->weight_log == 0) ? db_w / threshold : db_w;
            int is_decay = 0;
            double cur = 0;
            if (r->v_len > 0 && r->v_len < 192) {
                if (decay_value_at(rec_v(r), r->v_len, sum_now, &cur)) is_decay = 1;
            }
            if (is_decay && cur == 0) continue;
            raw_s++;
            if (r->v_len > 0) {
                if (is_decay) {
                    s += w * cur;
                    raw_sum += cur;
                } else if (r->v_len < 192) {
                    char buf[192] = {0};
                    memcpy(buf, rec_v(r), r->v_len);
                    double v = strtod(buf, NULL);
                    s += v * w;
                    raw_sum += v;
                }
                    }
                }
                if (offs) munmap(offs, count * 8);
                if (raw_s < 30) s = raw_sum;
                printf("%.17g\t%.17g\t%llu\n", s, raw_sum, (unsigned long long)raw_s);
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
        return do_search(argv[3], argc - 4, argv + 4);
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
