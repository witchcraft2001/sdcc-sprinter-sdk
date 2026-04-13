#include <sprinter/dss.h>

void dss_exit(u8 code) __naked {
    __asm
        ld      b, a
        ld      c, #0x41
        rst     #0x10
        ret
    __endasm;
}
