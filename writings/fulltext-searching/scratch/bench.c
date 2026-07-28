//bin/sh -c 'o="${0%.c}"; [ "$o" -nt "$0" ] || ${CC:-clang} -O3 -march=native -DNDEBUG "$0" -o "$o"; exec "$o" "$@"' "$0" "$@"; exit
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <time.h>
#include <sys/resource.h>

static double now() {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return ts.tv_sec + ts.tv_nsec * 1e-9;
}

static size_t peak_rss_mb() {
    struct rusage ru;
    getrusage(RUSAGE_SELF, &ru);
    return ru.ru_maxrss / 1024;
}

static unsigned char *read_file(const char *path, size_t *len) {
    FILE *f = fopen(path, "rb");
    if (!f) { perror(path); exit(1); }
    fseek(f, 0, SEEK_END);
    *len = ftell(f);
    fseek(f, 0, SEEK_SET);
    unsigned char *buf = malloc(*len + 2);
    fread(buf, 1, *len, f);
    buf[*len] = 0;
    buf[*len + 1] = 0;
    fclose(f);
    return buf;
}

/* ---- Brute force ---- */
static int brute_count(const unsigned char *t, size_t n, const char *p, int plen) {
    if (plen == 0 || (size_t)plen > n) return 0;
    int c = 0;
    const unsigned char *s = t, *e = t + n - plen;
    while (s <= e) {
        s = memmem(s, e - s + 1, p, plen);
        if (!s) break;
        c++;
        s++;
    }
    return c;
}

/* ---- Trigram index ---- */
static uint64_t *tri_arr;
static size_t tri_n;

static int cmp_u64(const void *a, const void *b) {
    uint64_t x = *(const uint64_t*)a, y = *(const uint64_t*)b;
    return (x > y) - (x < y);
}

static void tri_build(const unsigned char *t, size_t n) {
    tri_n = n >= 2 ? n - 2 : 0;
    tri_arr = malloc(tri_n * sizeof(uint64_t));
    for (size_t i = 0; i < tri_n; i++) {
        uint32_t tri = ((uint32_t)t[i] << 16) | ((uint32_t)t[i+1] << 8) | t[i+2];
        tri_arr[i] = ((uint64_t)tri << 32) | (uint32_t)i;
    }
    qsort(tri_arr, tri_n, sizeof(uint64_t), cmp_u64);
}

static size_t tri_freq(uint32_t tri) {
    size_t lo = 0, hi = tri_n;
    while (lo < hi) { size_t m = (lo+hi)/2; if ((tri_arr[m]>>32) < tri) lo = m+1; else hi = m; }
    size_t s = lo;
    lo = s; hi = tri_n;
    while (lo < hi) { size_t m = (lo+hi)/2; if ((tri_arr[m]>>32) <= tri) lo = m+1; else hi = m; }
    return lo - s;
}

static int tri_count(const unsigned char *t, size_t n, const char *p, int plen) {
    if (plen < 3) return brute_count(t, n, p, plen);
    if ((size_t)plen > n) return 0;
    uint32_t best_tri = 0;
    int best_off = 0;
    size_t best_cnt = (size_t)-1;
    for (int i = 0; i <= plen - 3; i++) {
        uint32_t tri = ((uint32_t)(unsigned char)p[i] << 16) | ((uint32_t)(unsigned char)p[i+1] << 8) | (unsigned char)p[i+2];
        size_t cnt = tri_freq(tri);
        if (cnt < best_cnt) { best_cnt = cnt; best_tri = tri; best_off = i; }
    }
    size_t lo = 0, hi = tri_n;
    while (lo < hi) { size_t m = (lo+hi)/2; if ((tri_arr[m]>>32) < best_tri) lo = m+1; else hi = m; }
    size_t s = lo;
    lo = s; hi = tri_n;
    while (lo < hi) { size_t m = (lo+hi)/2; if ((tri_arr[m]>>32) <= best_tri) lo = m+1; else hi = m; }
    int count = 0;
    for (size_t i = s; i < lo; i++) {
        uint32_t pos = (uint32_t)tri_arr[i];
        if (pos >= (uint32_t)best_off) {
            size_t sp = pos - best_off;
            if (sp + plen <= n && memcmp(t + sp, p, plen) == 0) count++;
        }
    }
    return count;
}

/* ---- Suffix array (prefix doubling) ---- */
typedef struct { int r1, r2, idx; } Triple;
static int cmp_tri(const void *a, const void *b) {
    const Triple *x = a, *y = b;
    if (x->r1 != y->r1) return x->r1 - y->r1;
    if (x->r2 != y->r2) return x->r2 - y->r2;
    return x->idx - y->idx;
}

static int *sa;
static size_t sa_n;

static void sa_build(const unsigned char *t, size_t n) {
    sa_n = n;
    sa = malloc(n * sizeof(int));
    int *rank = malloc(n * sizeof(int));
    int *tmp = malloc(n * sizeof(int));
    Triple *trips = malloc(n * sizeof(Triple));
    for (size_t i = 0; i < n; i++) { sa[i] = i; rank[i] = t[i]; }
    for (int k = 1; k < (int)n; k *= 2) {
        for (size_t i = 0; i < n; i++) {
            trips[i].r1 = rank[i];
            trips[i].r2 = (i + (size_t)k < n) ? rank[i + k] : -1;
            trips[i].idx = i;
        }
        qsort(trips, n, sizeof(Triple), cmp_tri);
        for (size_t i = 0; i < n; i++) sa[i] = trips[i].idx;
        tmp[sa[0]] = 0;
        for (size_t i = 1; i < n; i++)
            tmp[sa[i]] = tmp[sa[i-1]] + (trips[i].r1 != trips[i-1].r1 || trips[i].r2 != trips[i-1].r2);
        memcpy(rank, tmp, n * sizeof(int));
        if (rank[sa[n-1]] == (int)(n - 1)) break;
    }
    free(rank); free(tmp); free(trips);
}

static int sa_cmp(const unsigned char *t, size_t n, int sp, const char *p, int plen) {
    size_t avail = n - sp;
    int cl = plen < (int)avail ? plen : (int)avail;
    int c = memcmp(t + sp, p, cl);
    if (c) return c;
    return (int)avail < plen ? -1 : 0;
}

static int sa_count(const unsigned char *t, size_t n, const char *p, int plen) {
    if (plen == 0 || (size_t)plen > n) return 0;
    size_t lo = 0, hi = n;
    while (lo < hi) { size_t m = (lo+hi)/2; if (sa_cmp(t, n, sa[m], p, plen) < 0) lo = m+1; else hi = m; }
    size_t lb = lo;
    lo = lb; hi = n;
    while (lo < hi) { size_t m = (lo+hi)/2; if (sa_cmp(t, n, sa[m], p, plen) <= 0) lo = m+1; else hi = m; }
    return (int)(lo - lb);
}

/* ---- FM-index (BWT + position arrays for rank) ---- */
static unsigned char *bwt;
static int Ctab[257];
static int **occ_pos;
static int *occ_cnt;
static size_t fm_n;

static void fm_build(const unsigned char *t, size_t n) {
    fm_n = n + 1;
    bwt = malloc(fm_n);
    bwt[0] = t[n - 1];
    for (size_t i = 0; i < n; i++)
        bwt[i + 1] = sa[i] > 0 ? t[sa[i] - 1] : 0;
    int freq[256] = {0};
    for (size_t i = 0; i < n; i++) freq[t[i]]++;
    freq[0] = 1;
    Ctab[0] = 0;
    for (int i = 1; i < 257; i++) Ctab[i] = Ctab[i-1] + freq[i-1];
    occ_cnt = calloc(256, sizeof(int));
    for (size_t i = 0; i < fm_n; i++) occ_cnt[bwt[i]]++;
    occ_pos = calloc(256, sizeof(int*));
    for (int c = 0; c < 256; c++) occ_pos[c] = malloc(occ_cnt[c] * sizeof(int));
    int *fill = calloc(256, sizeof(int));
    for (size_t i = 0; i < fm_n; i++) occ_pos[bwt[i]][fill[bwt[i]]++] = i;
    free(fill);
}

static int fm_rank(int c, int pos) {
    int cnt = occ_cnt[c];
    if (cnt == 0) return 0;
    int lo = 0, hi = cnt;
    while (lo < hi) { int m = (lo+hi)/2; if (occ_pos[c][m] < pos) lo = m+1; else hi = m; }
    return lo;
}

static int fm_count(const unsigned char *t, size_t n, const char *p, int plen) {
    if (plen == 0 || (size_t)plen > n) return 0;
    int lo = 0, hi = (int)fm_n;
    for (int i = plen - 1; i >= 0; i--) {
        int c = (unsigned char)p[i];
        lo = Ctab[c] + fm_rank(c, lo);
        hi = Ctab[c] + fm_rank(c, hi);
        if (lo >= hi) return 0;
    }
    return hi - lo;
}

/* ---- Main ---- */
int main(int argc, char **argv) {
    if (argc < 2) { fprintf(stderr, "usage: %s corpus.txt\n", argv[0]); return 1; }
    size_t n;
    unsigned char *text = read_file(argv[1], &n);
    printf("corpus: %zu bytes (%.1f MB)\n\n", n, n / 1e6);

    printf("building trigram index...\n");
    double t0 = now();
    tri_build(text, n);
    double t1 = now();
    size_t tri_mem = tri_n * sizeof(uint64_t);
    printf("  trigram:     %.3f s, %.1f MB (%.1f%%)\n", t1-t0, tri_mem/1e6, tri_mem*100.0/n);

    printf("building suffix array...\n");
    t0 = now();
    sa_build(text, n);
    t1 = now();
    size_t sa_mem = sa_n * sizeof(int);
    printf("  suffix arr:  %.3f s, %.1f MB (%.1f%%)\n", t1-t0, sa_mem/1e6, sa_mem*100.0/n);

    printf("building FM-index...\n");
    t0 = now();
    fm_build(text, n);
    t1 = now();
    size_t fm_mem = fm_n;
    for (int c = 0; c < 256; c++) fm_mem += occ_cnt[c] * sizeof(int);
    printf("  FM-index:    %.3f s, %.1f MB (%.1f%%)\n", t1-t0, fm_mem/1e6, fm_mem*100.0/n);

    const char *patterns[] = {
        "the", "th", "index", "suffix", "bloom filter",
        "regular expression", "memory", "cache", "zxqwnonexist",
    };
    int npat = sizeof(patterns)/sizeof(patterns[0]);
    printf("\n%-25s %4s %10s %10s %10s %10s %8s\n", "pattern", "len", "brute", "trigram", "sa", "fm-index", "matches");
    printf("%-25s %4s %10s %10s %10s %10s %8s\n", "-------", "---", "-----", "-------", "--", "-------", "-------");

    for (int i = 0; i < npat; i++) {
        const char *p = patterns[i];
        int plen = strlen(p);
        int c1 = brute_count(text, n, p, plen);
        int c2 = tri_count(text, n, p, plen);
        int c3 = sa_count(text, n, p, plen);
        int c4 = fm_count(text, n, p, plen);
        if (c1 != c2 || c1 != c3 || c1 != c4) {
            printf("%-25s MISMATCH: brute=%d tri=%d sa=%d fm=%d\n", p, c1, c2, c3, c4);
            continue;
        }
            int reps_bf = 20, reps_tri = 100, reps_idx = 100000;
            volatile int sink = 0;
            t0 = now();
            for (int r = 0; r < reps_bf; r++) sink += brute_count(text, n, p, plen);
            t1 = now();
            double tb = (t1-t0)/reps_bf;
            t0 = now();
            for (int r = 0; r < reps_tri; r++) sink += tri_count(text, n, p, plen);
            t1 = now();
            double tt = (t1-t0)/reps_tri;
            t0 = now();
            for (int r = 0; r < reps_idx; r++) sink += sa_count(text, n, p, plen);
            t1 = now();
            double ts = (t1-t0)/reps_idx;
            t0 = now();
            for (int r = 0; r < reps_idx; r++) sink += fm_count(text, n, p, plen);
            t1 = now();
            double tf = (t1-t0)/reps_idx;
            (void)sink;
        printf("%-25s %4d %8.3fms %8.3fms %8.3fus %8.3fus %8d\n", p, plen, tb*1000, tt*1000, ts*1000000, tf*1000000, c1);
    }
    printf("\npeak RSS: %zu MB\n", peak_rss_mb());
    return 0;
}
