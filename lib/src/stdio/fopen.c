/* fopen.c — open a file */
#include <stdio.h>
#include <sprinter/dss.h>

FILE *fopen(const char *path, const char *mode) {
    int i;
    i16 fd;
    u8 flags = 0;

    /* Parse mode string */
    if (mode[0] == 'r') {
        flags = _F_READ;
        if (mode[1] == '+') flags |= _F_WRITE;
        fd = dss_open(path, (flags & _F_WRITE) ? O_RDWR : O_RDONLY);
    } else if (mode[0] == 'w') {
        flags = _F_WRITE;
        if (mode[1] == '+') flags |= _F_READ;
        fd = dss_creat(path);
    } else if (mode[0] == 'a') {
        flags = _F_WRITE;
        if (mode[1] == '+') flags |= _F_READ;
        fd = dss_open(path, O_RDWR);
        if (fd < 0) fd = dss_creat(path);
    } else {
        return NULL;
    }

    if (fd < 0) return NULL;

    /* Find free FILE slot (skip stdin/stdout/stderr = 0,1,2) */
    for (i = 3; i < FOPEN_MAX; i++) {
        if (!(_stdio_files[i].flags & _F_OPEN)) {
            _stdio_files[i].fd = (u8)fd;
            _stdio_files[i].flags = flags | _F_OPEN;
            _stdio_files[i].ungetc_buf = 0xFF;
            return &_stdio_files[i];
        }
    }
    /* No free slots — close fd and fail */
    dss_close((u8)fd);
    return NULL;
}
