/* fwrite.c — binary write to stream */
#include <stdio.h>
#include <sprinter/dss.h>

size_t fwrite(const void *buf, size_t size, size_t count, FILE *fp) {
    size_t total = size * count;
    i16 wrote;
    if (total == 0 || !(fp->flags & _F_OPEN) || fp->fd == 0xFF)
        return 0;
    wrote = dss_write(fp->fd, buf, total);
    if (wrote <= 0) {
        fp->flags |= _F_ERR;
        return 0;
    }
    return (size_t)wrote / size;
}
