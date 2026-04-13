/* printf.c — minimal printf for Z80/SDCC
 * Supports: %d %u %x %X %s %c %% %ld %lu %lx, width, zero-pad, '-'
 * No floating point.
 */
#include <sprinter/_printf.h>

static void _emit_char(int c, FILE *fp, int *cnt) {
    fputc(c, fp);
    (*cnt)++;
}

static void _emit_str(const char *s, FILE *fp, int *cnt) {
    while (*s) { _emit_char(*s++, fp, cnt); }
}

static void _emit_pad(int c, int n, FILE *fp, int *cnt) {
    while (n-- > 0) _emit_char(c, fp, cnt);
}

static void _utoa(unsigned long val, char *buf, int base, int upper) {
    char tmp[12];
    int i = 0;
    const char *digits = upper ? "0123456789ABCDEF" : "0123456789abcdef";
    if (val == 0) { tmp[i++] = '0'; }
    else while (val) { tmp[i++] = digits[val % base]; val /= base; }
    /* reverse */
    int j;
    for (j = 0; j < i; j++) buf[j] = tmp[i - 1 - j];
    buf[i] = '\0';
}

int _vprintfmt(FILE *fp, const char *fmt, va_list ap) {
    int cnt = 0;
    char numbuf[12];

    while (*fmt) {
        if (*fmt != '%') {
            _emit_char(*fmt++, fp, &cnt);
            continue;
        }
        fmt++; /* skip '%' */

        /* Flags */
        int left = 0, zeropad = 0;
        while (*fmt == '-' || *fmt == '0') {
            if (*fmt == '-') left = 1;
            if (*fmt == '0') zeropad = 1;
            fmt++;
        }
        if (left) zeropad = 0;

        /* Width */
        int width = 0;
        while (*fmt >= '0' && *fmt <= '9')
            width = width * 10 + (*fmt++ - '0');

        /* Long modifier */
        int islong = 0;
        if (*fmt == 'l') { islong = 1; fmt++; }

        /* Conversion */
        int len;
        const char *str;
        char ch;
        int neg = 0;
        unsigned long uval;

        switch (*fmt++) {
        case 'd':
        case 'i':
            if (islong) {
                long sv = va_arg(ap, long);
                if (sv < 0) { neg = 1; uval = (unsigned long)(-sv); }
                else uval = (unsigned long)sv;
            } else {
                int sv = va_arg(ap, int);
                if (sv < 0) { neg = 1; uval = (unsigned int)(-sv); }
                else uval = (unsigned int)sv;
            }
            _utoa(uval, numbuf + 1, 10, 0);
            if (neg) numbuf[0] = '-'; else { /* shift */ int k; for(k=0; numbuf[k+1]; k++) numbuf[k]=numbuf[k+1]; numbuf[k]=0; goto emit_num; }
            str = numbuf;
            goto emit_str;
        emit_num:
            str = numbuf;
            goto emit_str;

        case 'u':
            uval = islong ? va_arg(ap, unsigned long) : va_arg(ap, unsigned int);
            _utoa(uval, numbuf, 10, 0);
            str = numbuf;
            goto emit_str;

        case 'x':
            uval = islong ? va_arg(ap, unsigned long) : va_arg(ap, unsigned int);
            _utoa(uval, numbuf, 16, 0);
            str = numbuf;
            goto emit_str;

        case 'X':
            uval = islong ? va_arg(ap, unsigned long) : va_arg(ap, unsigned int);
            _utoa(uval, numbuf, 16, 1);
            str = numbuf;
            goto emit_str;

        case 's':
            str = va_arg(ap, const char *);
            if (!str) str = "(null)";
            goto emit_str;

        case 'c':
            ch = (char)va_arg(ap, int);
            _emit_char(ch, fp, &cnt);
            continue;

        case '%':
            _emit_char('%', fp, &cnt);
            continue;

        default:
            _emit_char('%', fp, &cnt);
            _emit_char(*(fmt-1), fp, &cnt);
            continue;
        }

    emit_str:
        /* Calculate length */
        len = 0;
        { const char *p = str; while (*p++) len++; }
        if (!left) _emit_pad(zeropad ? '0' : ' ', width - len, fp, &cnt);
        _emit_str(str, fp, &cnt);
        if (left) _emit_pad(' ', width - len, fp, &cnt);
    }
    return cnt;
}

int printf(const char *fmt, ...) {
    va_list ap;
    int r;
    va_start(ap, fmt);
    r = _vprintfmt(stdout, fmt, ap);
    va_end(ap);
    return r;
}

int fprintf(FILE *fp, const char *fmt, ...) {
    va_list ap;
    int r;
    va_start(ap, fmt);
    r = _vprintfmt(fp, fmt, ap);
    va_end(ap);
    return r;
}
