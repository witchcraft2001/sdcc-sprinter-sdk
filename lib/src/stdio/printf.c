/* printf.c — compact printf engine for Z80/SDCC
 *
 * Supports: %d %i %u %x %X %s %c %%, width, zero-pad, left-align
 * All integer conversions are 16-bit (int).
 *
 * %l modifier is accepted but ignored (prints lower 16 bits).
 * For full 32-bit %ld support, link with printf_long.rel.
 *
 * Typical code size: ~500 bytes (no 32-bit division pulled in).
 */
#include <sprinter/_printf.h>

static char *_utoa(unsigned int val, char *end, int base, int upper) {
    const char *d = upper ? "0123456789ABCDEF" : "0123456789abcdef";
    *end = '\0';
    do {
        *(--end) = d[val % base];
        val /= base;
    } while (val);
    return end;
}

int _vprintfmt(FILE *fp, const char *fmt, va_list ap) {
    int cnt = 0;
    char buf[8];
    char *p;
    int c;

    while ((c = *fmt++) != 0) {
        if (c != '%') { fputc(c, fp); cnt++; continue; }

        int left = 0, zero = 0;
        while (*fmt == '-' || *fmt == '0') {
            if (*fmt == '-') left = 1; else zero = 1;
            fmt++;
        }
        if (left) zero = 0;

        int w = 0;
        while (*fmt >= '0' && *fmt <= '9')
            w = w * 10 + (*fmt++ - '0');

        /* Accept and skip 'l' modifier — treats as int */
        if (*fmt == 'l') fmt++;

        int neg = 0;
        unsigned int uval;
        int len;

        switch ((c = *fmt++)) {
        case 'd': case 'i': {
            int sv = va_arg(ap, int);
            if (sv < 0) { neg = 1; sv = -sv; }
            p = _utoa((unsigned int)sv, buf + 7, 10, 0);
            if (neg) *(--p) = '-';
            break;
        }
        case 'u':
            p = _utoa(va_arg(ap, unsigned int), buf + 7, 10, 0);
            break;
        case 'x': case 'X':
            p = _utoa(va_arg(ap, unsigned int), buf + 7, 16, c == 'X');
            break;
        case 's':
            p = va_arg(ap, char *);
            if (!p) p = "(null)";
            break;
        case 'c':
            fputc(va_arg(ap, int), fp); cnt++;
            continue;
        case '%':
            fputc('%', fp); cnt++;
            continue;
        default:
            fputc('%', fp); fputc(c, fp); cnt += 2;
            continue;
        }

        len = 0; { char *q = p; while (*q++) len++; }
        char pad = zero ? '0' : ' ';
        if (!left) { int i; for (i = len; i < w; i++) { fputc(pad, fp); cnt++; } }
        while (*p) { fputc(*p++, fp); cnt++; }
        if (left)  { int i; for (i = len; i < w; i++) { fputc(' ', fp); cnt++; } }
    }
    return cnt;
}

int printf(const char *fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    int r = _vprintfmt(stdout, fmt, ap);
    va_end(ap);
    return r;
}

int fprintf(FILE *fp, const char *fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    int r = _vprintfmt(fp, fmt, ap);
    va_end(ap);
    return r;
}
