/* fopen.c — open a file */
#include <stdio.h>
#include <sprinter/dss.h>

#define _STDIO_PATH_MAX 80

static const char *_stdio_norm_path(const char *path, char *buf) {
    unsigned char i = 0;
    char c;

    while ((c = path[i]) != '\0') {
        if (i >= (_STDIO_PATH_MAX - 1))
            return path;
        if (c >= 'a' && c <= 'z')
            c -= ('a' - 'A');
        buf[i++] = c;
    }
    buf[i] = '\0';
    return buf;
}

static int _fopen_has_plus(const char *mode) {
    char c;

    while ((c = *++mode) != '\0') {
        if (c == '+') return 1;
    }
    return 0;
}

FILE *fopen(const char *path, const char *mode) {
    int i;
    i16 fd;
    u8 flags = 0;
    char path_buf[_STDIO_PATH_MAX];
    const char *dss_path = _stdio_norm_path(path, path_buf);

    /* Parse mode string */
    if (mode[0] == 'r') {
        flags = _F_READ;
        if (_fopen_has_plus(mode)) flags |= _F_WRITE;
        fd = dss_open(dss_path, (flags & _F_WRITE) ? O_RDWR : O_RDONLY);
    } else if (mode[0] == 'w') {
        flags = _F_WRITE;
        if (_fopen_has_plus(mode)) flags |= _F_READ;
        fd = dss_creat(dss_path);
    } else if (mode[0] == 'a') {
        flags = _F_WRITE | _F_APPEND;
        if (_fopen_has_plus(mode)) flags |= _F_READ;
        fd = dss_open(dss_path, O_RDWR);
        if (fd < 0) fd = dss_creat(dss_path);
        if (fd >= 0 && dss_seek((u8)fd, 0UL, SEEK_END) < 0) {
            dss_close((u8)fd);
            return NULL;
        }
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
