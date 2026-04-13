/* fclose.c */
#include <stdio.h>
#include <sprinter/dss.h>

int fclose(FILE *fp) {
    if (!(fp->flags & _F_OPEN))
        return EOF;
    if (fp->fd != 0xFF)
        dss_close(fp->fd);
    fp->flags = 0;
    fp->fd = 0xFF;
    return 0;
}
