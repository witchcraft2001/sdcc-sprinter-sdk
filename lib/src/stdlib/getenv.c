#include <sprinter/dss.h>

static char _getenv_buf[64];

char *getenv(const char *name) {
    if (dss_getenv(name, _getenv_buf) == 0)
        return _getenv_buf;
    return (char *)0;
}
