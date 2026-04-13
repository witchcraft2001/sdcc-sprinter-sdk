/* sprintf.c — format to string buffer
 * Uses a fake FILE with fd=0xFE that fputc handles specially.
 */
#include <sprinter/_printf.h>

/* Global pointer for sprintf output (not reentrant, fine for Z80) */
char *_sprintf_ptr;

int sprintf(char *buf, const char *fmt, ...) {
    va_list ap;
    FILE fake;
    int r;
    _sprintf_ptr = buf;
    fake.fd = 0xFE;
    fake.flags = _F_WRITE | _F_OPEN;
    fake.ungetc_buf = 0xFF;
    va_start(ap, fmt);
    r = _vprintfmt(&fake, fmt, ap);
    va_end(ap);
    *_sprintf_ptr = '\0';
    return r;
}
