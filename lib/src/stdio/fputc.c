/* fputc.c — write one character to stream */
#include <stdio.h>
#include <sprinter/dss.h>

/* sprintf buffer pointer (defined in sprintf.c) */
extern char *_sprintf_ptr;

int fputc(int c, FILE *fp) {
    if (!(fp->flags & _F_OPEN) || !(fp->flags & _F_WRITE))
        return EOF;
    if (fp->fd == 0xFF) {
        /* Console output — text mode: \n -> \r\n */
        if (c == '\n') dss_putchar('\r');
        dss_putchar((u8)c);
    } else if (fp->fd == 0xFE) {
        /* sprintf buffer */
        *_sprintf_ptr++ = (char)c;
    } else {
        u8 ch = (u8)c;
        if (dss_write(fp->fd, &ch, 1) < 0) {
            fp->flags |= _F_ERR;
            return EOF;
        }
    }
    return (unsigned char)c;
}
