#if 0
o=${0%.c}
[ "$o" -nt "$0" ] || cc -O3 -std=c11 -Wall -Wextra "$0" -o "$o"
exec "$o" "$@"
#endif
#define _POSIX_C_SOURCE 200809L
#include <ctype.h>
#include <errno.h>
#include <fcntl.h>
#include <inttypes.h>
#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>

enum { MAX_COLS = 32, NAME_SIZE = 32 };

typedef struct {
    char magic[8];
    uint64_t rows;
    uint32_t cols;
    char names[MAX_COLS][NAME_SIZE];
} Header;

typedef struct {
    int fd;
    size_t size;
    Header *header;
    int64_t *data;
} Db;

typedef struct {
    int enabled;
    uint32_t col;
    int64_t lo;
    int64_t hi;
} Filter;

typedef struct {
    int used;
    int64_t key;
    uint64_t count;
    __int128 sum;
} Group;

static void usage(void);
static void fail(const char *format, ...);
static void load(const char *path);
static void info(const char *path);
static void stats(int argc, char **argv);
static void groups(int argc, char **argv);
static Db open_db(const char *path);
static void close_db(Db *db);
static uint32_t find_col(Db *db, const char *name);
static int64_t *column(Db *db, uint32_t col);
static Filter filter_from(Db *db, int argc, char **argv, int at);
static int passes(Db *db, Filter filter, uint64_t row);
static int64_t parse_i64(const char *text, const char *object);
static size_t split(char *line, char delimiter, char **fields);
static char *trim(char *text);
static void sync_file(FILE *file, const char *path);
static uint64_t hash64(uint64_t value);
static int by_key(const void *left, const void *right);
static void print_i128(__int128 value);

int main(int argc, char **argv)
{
    if (argc == 3 && !strcmp(argv[1], "load")) {
        load(argv[2]);
        return 0;
    }
    if (argc == 3 && !strcmp(argv[1], "info")) {
        info(argv[2]);
        return 0;
    }
    if ((argc == 4 || argc == 7) && !strcmp(argv[1], "stats")) {
        stats(argc, argv);
        return 0;
    }
    if ((argc == 5 || argc == 8) && !strcmp(argv[1], "group")) {
        groups(argc, argv);
        return 0;
    }
    usage();
    return 2;
}

static void usage(void)
{
    fputs(
        "usage:\n"
        "  ch load DB < integers.tsv\n"
        "  ch info DB\n"
        "  ch stats DB VALUE [WHERE_COLUMN LO HI]\n"
        "  ch group DB KEY VALUE [WHERE_COLUMN LO HI]\n",
        stderr
    );
}

static void fail(const char *format, ...)
{
    va_list arguments;

    va_start(arguments, format);
    vfprintf(stderr, format, arguments);
    va_end(arguments);
    fputc('\n', stderr);
    exit(1);
}

static void load(const char *path)
{
    Header header = {0};
    int64_t *columns[MAX_COLS] = {0};
    char *line = 0;
    size_t line_size = 0;
    char *fields[MAX_COLS + 1];
    size_t count;
    size_t capacity = 0;
    char delimiter;
    char *new_path;
    FILE *file;

    if (getline(&line, &line_size, stdin) < 0) {
        fail("load %s: missing header", path);
    }
    delimiter = strchr(line, '\t') ? '\t' : ',';
    count = split(line, delimiter, fields);
    if (!count || count > MAX_COLS) {
        fail("load %s: columns must be between 1 and %d", path, MAX_COLS);
    }
    memcpy(header.magic, "CH20v1", 7);
    header.cols = count;
    for (size_t col = 0; col < count; col++) {
        char *name = trim(fields[col]);

        if (!*name || strlen(name) >= NAME_SIZE) {
            fail("load %s: bad column name: %s", path, name);
        }
        for (size_t previous = 0; previous < col; previous++) {
            if (!strcmp(header.names[previous], name)) {
                fail("load %s: duplicate column: %s", path, name);
            }
        }
        strcpy(header.names[col], name);
    }
    while (getline(&line, &line_size, stdin) >= 0) {
        size_t found = split(line, delimiter, fields);

        if (found == 1 && !*trim(fields[0])) {
            continue;
        }
        if (found != count) {
            fail(
                "load %s: row %" PRIu64 " has %zu fields, expected %zu",
                path,
                header.rows + 2,
                found,
                count
            );
        }
        if (header.rows == capacity) {
            size_t next = capacity ? capacity * 2 : 65536;

            if (next < capacity || next > SIZE_MAX / sizeof(int64_t)) {
                fail("load %s: table too large", path);
            }
            for (size_t col = 0; col < count; col++) {
                void *grown = realloc(columns[col], next * sizeof(int64_t));

                if (!grown) {
                    fail("load %s: realloc: %s", path, strerror(errno));
                }
                columns[col] = grown;
            }
            capacity = next;
        }
        for (size_t col = 0; col < count; col++) {
            columns[col][header.rows] = parse_i64(
                trim(fields[col]),
                header.names[col]
            );
        }
        header.rows++;
    }
    if (ferror(stdin)) {
        fail("load %s: read stdin: %s", path, strerror(errno));
    }
    if (header.rows > (SIZE_MAX - sizeof(header)) / count / sizeof(int64_t)) {
        fail("load %s: table too large", path);
    }
    new_path = malloc(strlen(path) + 5);
    if (!new_path) {
        fail("load %s: malloc: %s", path, strerror(errno));
    }
    sprintf(new_path, "%s.new", path);
    file = fopen(new_path, "wb");
    if (!file) {
        fail("open %s: %s", new_path, strerror(errno));
    }
    if (fwrite(&header, sizeof(header), 1, file) != 1) {
        fail("write %s header: %s", new_path, strerror(errno));
    }
    for (size_t col = 0; col < count; col++) {
        if (fwrite(columns[col], sizeof(int64_t), header.rows, file) != header.rows) {
            fail("write %s column %s: %s", new_path, header.names[col], strerror(errno));
        }
    }
    sync_file(file, new_path);
    if (fclose(file)) {
        fail("close %s: %s", new_path, strerror(errno));
    }
    if (rename(new_path, path)) {
        fail("rename %s to %s: %s", new_path, path, strerror(errno));
    }
    fprintf(
        stderr,
        "loaded rows=%" PRIu64 " columns=%u bytes=%zu db=%s\n",
        header.rows,
        header.cols,
        (size_t)(sizeof(header) + header.rows * header.cols * sizeof(int64_t)),
        path
    );
    free(new_path);
    free(line);
    for (size_t col = 0; col < count; col++) {
        free(columns[col]);
    }
}

static void info(const char *path)
{
    Db db = open_db(path);

    printf("rows=%" PRIu64 " columns=%u bytes=%zu\n", db.header->rows, db.header->cols, db.size);
    for (uint32_t col = 0; col < db.header->cols; col++) {
        printf("%u\t%s\n", col, db.header->names[col]);
    }
    close_db(&db);
}

static void stats(int argc, char **argv)
{
    Db db = open_db(argv[2]);
    uint32_t value_col = find_col(&db, argv[3]);
    int64_t *values = column(&db, value_col);
    Filter filter = filter_from(&db, argc, argv, 4);
    uint64_t count = 0;
    __int128 sum = 0;
    int64_t minimum = INT64_MAX;
    int64_t maximum = INT64_MIN;

    for (uint64_t row = 0; row < db.header->rows; row++) {
        int64_t value;

        if (!passes(&db, filter, row)) {
            continue;
        }
        value = values[row];
        count++;
        sum += value;
        if (value < minimum) {
            minimum = value;
        }
        if (value > maximum) {
            maximum = value;
        }
    }
    printf("count=%" PRIu64 " sum=", count);
    print_i128(sum);
    if (count) {
        printf(" min=%" PRId64 " max=%" PRId64 "\n", minimum, maximum);
    } else {
        fputs(" min=null max=null\n", stdout);
    }
    close_db(&db);
}

static void groups(int argc, char **argv)
{
    Db db = open_db(argv[2]);
    int64_t *keys = column(&db, find_col(&db, argv[3]));
    int64_t *values = column(&db, find_col(&db, argv[4]));
    Filter filter = filter_from(&db, argc, argv, 5);
    uint64_t selected = 0;
    size_t capacity = 16;
    Group *table;
    Group *output;
    size_t groups_count = 0;

    for (uint64_t row = 0; row < db.header->rows; row++) {
        selected += passes(&db, filter, row);
    }
    while (capacity < selected * 2) {
        if (capacity > SIZE_MAX / 2 / sizeof(Group)) {
            fail("group %s: result too large", argv[2]);
        }
        capacity *= 2;
    }
    table = calloc(capacity, sizeof(Group));
    if (!table) {
        fail("group %s: calloc: %s", argv[2], strerror(errno));
    }
    for (uint64_t row = 0; row < db.header->rows; row++) {
        size_t slot;

        if (!passes(&db, filter, row)) {
            continue;
        }
        slot = hash64(keys[row]) & (capacity - 1);
        while (table[slot].used && table[slot].key != keys[row]) {
            slot = (slot + 1) & (capacity - 1);
        }
        if (!table[slot].used) {
            table[slot].used = 1;
            table[slot].key = keys[row];
            groups_count++;
        }
        table[slot].count++;
        table[slot].sum += values[row];
    }
    output = malloc((groups_count ? groups_count : 1) * sizeof(Group));
    if (!output) {
        fail("group %s: malloc: %s", argv[2], strerror(errno));
    }
    groups_count = 0;
    for (size_t slot = 0; slot < capacity; slot++) {
        if (table[slot].used) {
            output[groups_count++] = table[slot];
        }
    }
    qsort(output, groups_count, sizeof(Group), by_key);
    printf("%s\tcount\tsum_%s\n", argv[3], argv[4]);
    for (size_t index = 0; index < groups_count; index++) {
        printf("%" PRId64 "\t%" PRIu64 "\t", output[index].key, output[index].count);
        print_i128(output[index].sum);
        putchar('\n');
    }
    free(output);
    free(table);
    close_db(&db);
}

static Db open_db(const char *path)
{
    Db db = {.fd = -1};
    struct stat status;
    size_t expected;

    db.fd = open(path, O_RDONLY);
    if (db.fd < 0) {
        fail("open %s: %s", path, strerror(errno));
    }
    if (fstat(db.fd, &status)) {
        fail("stat %s: %s", path, strerror(errno));
    }
    if (status.st_size < (off_t)sizeof(Header)) {
        fail("open %s: truncated header", path);
    }
    db.size = status.st_size;
    db.header = mmap(0, db.size, PROT_READ, MAP_PRIVATE, db.fd, 0);
    if (db.header == MAP_FAILED) {
        fail("mmap %s: %s", path, strerror(errno));
    }
    if (memcmp(db.header->magic, "CH20v1", 7)) {
        fail("open %s: bad magic", path);
    }
    if (!db.header->cols || db.header->cols > MAX_COLS) {
        fail("open %s: bad column count: %u", path, db.header->cols);
    }
    if (db.header->rows > (SIZE_MAX - sizeof(Header)) / db.header->cols / sizeof(int64_t)) {
        fail("open %s: size overflow", path);
    }
    expected = sizeof(Header) + db.header->rows * db.header->cols * sizeof(int64_t);
    if (expected != db.size) {
        fail("open %s: size=%zu expected=%zu", path, db.size, expected);
    }
    db.data = (int64_t *)((char *)db.header + sizeof(Header));
    return db;
}

static void close_db(Db *db)
{
    if (munmap(db->header, db->size)) {
        fail("munmap database: %s", strerror(errno));
    }
    if (close(db->fd)) {
        fail("close database: %s", strerror(errno));
    }
}

static uint32_t find_col(Db *db, const char *name)
{
    for (uint32_t col = 0; col < db->header->cols; col++) {
        if (!strcmp(db->header->names[col], name)) {
            return col;
        }
    }
    fail("unknown column: %s", name);
    return 0;
}

static int64_t *column(Db *db, uint32_t col)
{
    return db->data + col * db->header->rows;
}

static Filter filter_from(Db *db, int argc, char **argv, int at)
{
    Filter filter = {0};

    if (argc == at) {
        return filter;
    }
    filter.enabled = 1;
    filter.col = find_col(db, argv[at]);
    filter.lo = parse_i64(argv[at + 1], "filter lower bound");
    filter.hi = parse_i64(argv[at + 2], "filter upper bound");
    if (filter.lo > filter.hi) {
        fail(
            "filter lower bound exceeds upper bound: %" PRId64 " > %" PRId64,
            filter.lo,
            filter.hi
        );
    }
    return filter;
}

static int passes(Db *db, Filter filter, uint64_t row)
{
    int64_t value;

    if (!filter.enabled) {
        return 1;
    }
    value = column(db, filter.col)[row];
    return value >= filter.lo && value <= filter.hi;
}

static int64_t parse_i64(const char *text, const char *object)
{
    char *end;
    int64_t value;

    errno = 0;
    value = strtoll(text, &end, 10);
    while (isspace((unsigned char)*end)) {
        end++;
    }
    if (errno || end == text || *end) {
        fail("bad integer for %s: %s", object, text);
    }
    return value;
}

static size_t split(char *line, char delimiter, char **fields)
{
    size_t count = 1;

    fields[0] = line;
    for (char *cursor = line; *cursor; cursor++) {
        if (*cursor == '\n' || *cursor == '\r') {
            *cursor = 0;
            break;
        }
        if (*cursor == delimiter) {
            *cursor = 0;
            if (count > MAX_COLS) {
                return count + 1;
            }
            fields[count++] = cursor + 1;
        }
    }
    return count;
}

static char *trim(char *text)
{
    char *end;

    while (isspace((unsigned char)*text)) {
        text++;
    }
    end = text + strlen(text);
    while (end > text && isspace((unsigned char)end[-1])) {
        *--end = 0;
    }
    return text;
}

static void sync_file(FILE *file, const char *path)
{
    if (fflush(file)) {
        fail("flush %s: %s", path, strerror(errno));
    }
#if defined(__APPLE__) && defined(F_FULLFSYNC)
    if (fcntl(fileno(file), F_FULLFSYNC)) {
        fail("full sync %s: %s", path, strerror(errno));
    }
#else
    if (fsync(fileno(file))) {
        fail("sync %s: %s", path, strerror(errno));
    }
#endif
}

static uint64_t hash64(uint64_t value)
{
    value ^= value >> 30;
    value *= UINT64_C(0xbf58476d1ce4e5b9);
    value ^= value >> 27;
    value *= UINT64_C(0x94d049bb133111eb);
    return value ^ (value >> 31);
}

static int by_key(const void *left, const void *right)
{
    const Group *a = left;
    const Group *b = right;

    return (a->key > b->key) - (a->key < b->key);
}

static void print_i128(__int128 value)
{
    char digits[40];
    size_t count = 0;
    unsigned __int128 magnitude;

    if (value < 0) {
        putchar('-');
        magnitude = (unsigned __int128)(-(value + 1)) + 1;
    } else {
        magnitude = value;
    }
    do {
        digits[count++] = '0' + magnitude % 10;
        magnitude /= 10;
    } while (magnitude);
    while (count) {
        putchar(digits[--count]);
    }
}
