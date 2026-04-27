/* fwrite.c — binary write to stream */
#include <stdio.h>
#include <sprinter/dss.h>

size_t fwrite(const void *buf, size_t size, size_t count, FILE *fp) {
    size_t total = size * count;
    i16 wrote;
    if (total == 0 || !(fp->flags & _F_OPEN) || !(fp->flags & _F_WRITE) || fp->fd == 0xFF)
        return 0;
    if ((fp->flags & _F_APPEND) && dss_seek(fp->fd, 0UL, SEEK_END) < 0) {
        fp->flags |= _F_ERR;
        return 0;
    }
    wrote = dss_write(fp->fd, buf, total);
    if (wrote <= 0) {
        fp->flags |= _F_ERR;
        return 0;
    }
    return (size_t)wrote / size;
}
