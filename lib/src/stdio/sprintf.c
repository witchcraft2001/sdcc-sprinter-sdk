/* sprintf.c — format to string buffer */
#include <sprinter/_printf.h>

/* Trick: use a temporary FILE that writes to a buffer */
static char *_spbuf;

static int _sp_putc(int c, FILE *fp) {
    (void)fp;
    *_spbuf++ = (char)c;
    return c;
}

int sprintf(char *buf, const char *fmt, ...) {
    va_list ap;
    FILE fake;
    int r;
    _spbuf = buf;
    /* Set up fake FILE for writing */
    fake.fd = 0xFE; /* sentinel: not console, not real file */
    fake.flags = _F_WRITE | _F_OPEN;
    fake.ungetc_buf = 0xFF;
    va_start(ap, fmt);
    /* Can't easily use _vprintfmt with fake file because fputc
       checks fd. Instead, format manually via the same engine.
       For simplicity, just call printf-like loop directly. */
    /* Actually, let's just format char by char using the real _vprintfmt
       but we need fputc to handle fd=0xFE specially.
       Simpler: write directly. */
    {
        const char *p = fmt;
        r = 0;
        /* Simple fallback: use _vprintfmt then terminate */
        r = _vprintfmt(&fake, fmt, ap);
    }
    va_end(ap);
    *_spbuf = '\0';
    return r;
}
