/* fputc.c — write one character to stream */
#include <stdio.h>
#include <sprinter/dss.h>

int fputc(int c, FILE *fp) {
    if (!(fp->flags & _F_OPEN) || !(fp->flags & _F_WRITE))
        return EOF;
    if (fp->fd == 0xFF) {
        /* Console output */
        dss_putchar((u8)c);
    } else if (fp->fd == 0xFE) {
        /* sprintf buffer — handled by caller, nothing to do */
        /* This path is actually not reached; sprintf overrides */
    } else {
        u8 ch = (u8)c;
        if (dss_write(fp->fd, &ch, 1) < 0) {
            fp->flags |= _F_ERR;
            return EOF;
        }
    }
    return (unsigned char)c;
}
