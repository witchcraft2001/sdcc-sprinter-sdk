#include <sprinter/dss.h>

extern u16 _cmdline;

char *dss_cmdline(void) {
    /* PSP format: byte 0 = length, byte 1+ = command tail text.
       Skip the length byte to return pointer to actual text. */
    return (char *)(_cmdline + 1);
}
