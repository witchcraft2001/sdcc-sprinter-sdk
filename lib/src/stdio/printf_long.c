/* printf_long.c — 32-bit number formatting for %ld/%lu/%lx
 * Only linked if printf encounters %l format specifiers.
 * This module pulls in _divulong/_modulong (~260 bytes).
 */

/* 32-bit unsigned to string — called from _vprintfmt when %l is used */
char *_utoa32(unsigned long val, char *end, int base, int upper) {
    const char *d = upper ? "0123456789ABCDEF" : "0123456789abcdef";
    *end = '\0';
    do {
        *(--end) = d[(unsigned int)(val % base)];
        val /= base;
    } while (val);
    return end;
}
