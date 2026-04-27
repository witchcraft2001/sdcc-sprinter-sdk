/* remove_rename.c — file management */
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

int remove(const char *path) {
    char path_buf[_STDIO_PATH_MAX];
    return dss_delete(_stdio_norm_path(path, path_buf)) ? -1 : 0;
}

int rename(const char *oldpath, const char *newpath) {
    char old_buf[_STDIO_PATH_MAX];
    char new_buf[_STDIO_PATH_MAX];

    return dss_rename(_stdio_norm_path(oldpath, old_buf),
                      _stdio_norm_path(newpath, new_buf)) ? -1 : 0;
}
