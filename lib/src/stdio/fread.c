/* fread.c — binary read from stream */
#include <stdio.h>
#include <sprinter/dss.h>

size_t fread(void *buf, size_t size, size_t count, FILE *fp) {
    size_t total = size * count;
    unsigned char *p = (unsigned char *)buf;
    size_t done = 0;
    i16 got;
    if (total == 0 || !(fp->flags & _F_OPEN) || !(fp->flags & _F_READ) || fp->fd == 0xFF)
        return 0;

    if (fp->ungetc_buf != 0xFF) {
        *p++ = fp->ungetc_buf;
        fp->ungetc_buf = 0xFF;
        done = 1;
        total--;
        if (total == 0)
            return done / size;
    }

    got = dss_read(fp->fd, p, total);
    if (got <= 0) {
        fp->flags |= _F_EOF;
        return done / size;
    }
    return (done + (size_t)got) / size;
}
