#include <sprinter/dss.h>

u16 dss_version(void) __naked {
    __asm
        push    ix
        ld      c, #0x00
        rst     #0x10
        pop     ix
        ret
    __endasm;
}
