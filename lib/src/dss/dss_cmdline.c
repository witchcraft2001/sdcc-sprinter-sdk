#include <sprinter/dss.h>

extern u16 _cmdline;

char *dss_cmdline(void) {
    /* PSP format: byte 0 = length, byte 1+ = command tail text. */
    u8 *psp = (u8 *)_cmdline;
    u8 len = psp[0];
    char *tail = (char *)(psp + 1);

    tail[len] = '\0';
    while (*tail == ' ') {
        ++tail;
    }
    return tail;
}
