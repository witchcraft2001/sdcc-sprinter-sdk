/* fread.c — binary read from stream */
#include <stdio.h>
#include <sprinter/dss.h>

size_t fread(void *buf, size_t size, size_t count, FILE *fp) {
    size_t total = size * count;
    i16 got;
    if (total == 0 || !(fp->flags & _F_OPEN) || fp->fd == 0xFF)
        return 0;
    got = dss_read(fp->fd, buf, total);
    if (got <= 0) {
        fp->flags |= _F_EOF;
        return 0;
    }
    return (size_t)got / size;
}
