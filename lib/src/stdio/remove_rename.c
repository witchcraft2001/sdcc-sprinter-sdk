/* remove_rename.c — file management */
#include <stdio.h>
#include <sprinter/dss.h>

int remove(const char *path) {
    return dss_delete(path) ? -1 : 0;
}
