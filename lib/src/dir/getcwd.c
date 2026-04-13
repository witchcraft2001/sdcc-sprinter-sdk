#include <sprinter/dss.h>

char *getcwd(char *buf, int size) {
    (void)size;
    if (dss_curdir(buf) == 0)
        return buf;
    return (char *)0;
}
