/* fseek.c */
#include <stdio.h>
#include <sprinter/dss.h>

int fseek(FILE *fp, long offset, int whence) {
    if (!(fp->flags & _F_OPEN) || fp->fd == 0xFF)
        return -1;
    fp->flags &= ~_F_EOF;
    fp->ungetc_buf = 0xFF;
    return dss_seek(fp->fd, (u32)offset, (u8)whence);
}
