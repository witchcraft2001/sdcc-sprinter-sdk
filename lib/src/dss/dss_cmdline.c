#include <sprinter/dss.h>

extern u16 _cmdline;

char *dss_cmdline(void) {
    return (char *)_cmdline;
}
