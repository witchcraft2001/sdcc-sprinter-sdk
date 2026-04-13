/*
 * bench.c - Z80 Benchmark Suite
 *
 * Portable C89 for cross-compiler comparison.
 * Uses only 8/16-bit integer arithmetic, no long.
 *
 * To port to another compiler/platform:
 *   - adjust #include and timer functions below
 *   - all tests use standard C only
 *
 * Sprinter 7 MHz: total ~30-50 sec (depends on compiler)
 */

#include <stdio.h>
#include <string.h>

/* ==== Platform timer (adjust for your compiler) ==== */
#include <dos.h>     /* gettime() — SDCC SDK / SOLID C / Turbo C */

static unsigned int _t0;

static void timer_start(void) {
    struct time t;
    unsigned char prev;
    gettime(&t);
    prev = t.ti_sec;
    do { gettime(&t); } while (t.ti_sec == prev);
    _t0 = (unsigned int)t.ti_min * 60 + t.ti_sec;
}

static unsigned int timer_elapsed(void) {
    struct time t;
    unsigned int t1;
    gettime(&t);
    t1 = (unsigned int)t.ti_min * 60 + t.ti_sec;
    if (t1 < _t0) t1 += 3600;   /* hour wrap */
    return t1 - _t0;
}
/* ==== End platform-specific ==== */

/* PRNG: LCG with period 256, deterministic fill */
static unsigned char rng;
static void    rng_seed(unsigned char s) { rng = s; }
static unsigned char rng_next(void) { rng = rng * 5 + 1; return rng; }

/* ---- Global arrays (stack is small on Z80) ---- */
#define SIEVE_N   1000
#define SORT_N     256
#define DATA_N     256

static unsigned char sieve[SIEVE_N];
static unsigned char arr[SORT_N];
static unsigned char data[DATA_N];
static unsigned char rc4_s[256];

/* ================================================ */
/*  Test 1: Sieve of Eratosthenes                  */
/*  Tests: array fill, inner loops, branching       */
/* ================================================ */
static unsigned int bench_sieve(int runs) {
    unsigned int i, j, count;
    int r;
    for (r = 0; r < runs; r++) {
        memset(sieve, 1, SIEVE_N);
        sieve[0] = sieve[1] = 0;
        for (i = 2; i * i < SIEVE_N; i++) {
            if (sieve[i]) {
                for (j = i * i; j < SIEVE_N; j += i)
                    sieve[j] = 0;
            }
        }
    }
    count = 0;
    for (i = 0; i < SIEVE_N; i++)
        if (sieve[i]) count++;
    return count;   /* expect 168 */
}

/* ================================================ */
/*  Test 2: Bubble Sort (256 bytes)                 */
/*  Tests: comparisons, swaps, array access         */
/* ================================================ */
static unsigned int bench_bubble(int runs) {
    int r, i, j;
    unsigned char t, sum;
    for (r = 0; r < runs; r++) {
        rng_seed(0x42);
        for (i = 0; i < SORT_N; i++) arr[i] = rng_next();
        for (i = 0; i < SORT_N - 1; i++)
            for (j = 0; j < SORT_N - 1 - i; j++)
                if (arr[j] > arr[j + 1]) {
                    t = arr[j]; arr[j] = arr[j + 1]; arr[j + 1] = t;
                }
    }
    sum = 0;
    for (i = 0; i < SORT_N; i++) sum += arr[i];
    return sum;     /* expect 128 */
}

/* ================================================ */
/*  Test 3: Shell Sort (256 bytes)                  */
/*  Tests: complex indexing, gap sequence           */
/* ================================================ */
static unsigned int bench_shell(int runs) {
    int r, i, j, gap;
    unsigned char t, sum;
    for (r = 0; r < runs; r++) {
        rng_seed(0x42);
        for (i = 0; i < SORT_N; i++) arr[i] = rng_next();
        for (gap = SORT_N / 2; gap > 0; gap /= 2)
            for (i = gap; i < SORT_N; i++) {
                t = arr[i];
                for (j = i; j >= gap && arr[j - gap] > t; j -= gap)
                    arr[j] = arr[j - gap];
                arr[j] = t;
            }
    }
    sum = 0;
    for (i = 0; i < SORT_N; i++) sum += arr[i];
    return sum;     /* expect 128 (same sorted data) */
}

/* ================================================ */
/*  Test 4: CRC-16 CCITT (256 bytes x N)            */
/*  Tests: bit shifts, XOR, byte-level loops        */
/* ================================================ */
static unsigned int crc16_calc(unsigned char *buf, unsigned int len) {
    unsigned int crc, i;
    unsigned char j, b;
    crc = 0xFFFF;
    for (i = 0; i < len; i++) {
        b = buf[i];
        for (j = 0; j < 8; j++) {
            if ((crc ^ b) & 1)
                crc = (crc >> 1) ^ 0x8408;
            else
                crc >>= 1;
            b >>= 1;
        }
    }
    return crc ^ 0xFFFF;
}

static unsigned int bench_crc(int runs) {
    int r;
    unsigned int i, result;
    for (i = 0; i < DATA_N; i++) data[i] = (unsigned char)i;
    result = 0;
    for (r = 0; r < runs; r++)
        result = crc16_calc(data, DATA_N);
    return result;  /* expect 47282 */
}

/* ================================================ */
/*  Test 5: RC4 stream cipher                       */
/*  Tests: byte permutation, indirect indexing      */
/* ================================================ */
static unsigned int bench_rc4(int runs) {
    int r;
    unsigned int n;
    unsigned char i, j, t, sum;
    for (r = 0; r < runs; r++) {
        /* KSA - Key Scheduling */
        for (n = 0; n < 256; n++) rc4_s[n] = (unsigned char)n;
        j = 0;
        for (n = 0; n < 256; n++) {
            i = (unsigned char)n;
            j = j + rc4_s[i] + (unsigned char)(n * 7 + 0x1F);
            t = rc4_s[i]; rc4_s[i] = rc4_s[j]; rc4_s[j] = t;
        }
        /* PRGA - encrypt 256 bytes */
        i = j = 0;
        for (n = 0; n < 256; n++) {
            i++;
            j += rc4_s[i];
            t = rc4_s[i]; rc4_s[i] = rc4_s[j]; rc4_s[j] = t;
            data[n] = rc4_s[(unsigned char)(rc4_s[i] + rc4_s[j])];
        }
    }
    sum = 0;
    for (n = 0; n < 256; n++) sum += data[n];
    return sum;
}

/* ================================================ */
/*  Test 6: 16-bit integer arithmetic               */
/*  Tests: multiply, add, modulo                    */
/* ================================================ */
static unsigned int bench_math(int runs) {
    unsigned int i, sum, x;
    int r;
    sum = 0;
    for (r = 0; r < runs; r++) {
        x = 1;
        for (i = 1; i <= 1000; i++) {
            x = x * 3 + i;         /* multiply + add */
            sum += x;
        }
    }
    return sum;
}

/* ================================================ */
/*  Main — run all tests, print results             */
/* ================================================ */
static void run_test(const char *name, unsigned int (*fn)(int),
                     int runs, unsigned int *total)
{
    unsigned int check, secs;
    printf("  %-12s x%-4d ", name, runs);
    timer_start();
    check = fn(runs);
    secs = timer_elapsed();
    *total += secs;
    printf(" %5u  %2u:%02u\n", check, secs / 60, secs % 60);
}

void main(void) {
    unsigned int total = 0;

    printf("Z80 Benchmark v1.0\n");
    printf("===================\n\n");
    printf("  Test          Runs  Check  Time\n");
    printf("  ------------ ----- ------ -----\n");

    run_test("Sieve",       bench_sieve,  200, &total);
    run_test("Bubble sort", bench_bubble,  10, &total);
    run_test("Shell sort",  bench_shell,   60, &total);
    run_test("CRC-16",      bench_crc,    400, &total);
    run_test("RC4",         bench_rc4,    500, &total);
    run_test("Int math",    bench_math,   200, &total);

    printf("  ------------ ----- ------ -----\n");
    printf("  TOTAL                     %2u:%02u\n", total / 60, total % 60);
}
