/* fgetc.c — read one character from stream */
#include <stdio.h>
#include <sprinter/dss.h>

int fgetc(FILE *fp) {
    u8 ch;
    if (!(fp->flags & _F_OPEN) || !(fp->flags & _F_READ))
        return EOF;
    if (fp->ungetc_buf != 0xFF) {
        ch = fp->ungetc_buf;
        fp->ungetc_buf = 0xFF;
        return ch;
    }
    if (fp->fd == 0xFF) {
        /* Console input */
        return dss_waitkey();
    }
    if (dss_read(fp->fd, &ch, 1) <= 0) {
        fp->flags |= _F_EOF;
        return EOF;
    }
    return ch;
}
