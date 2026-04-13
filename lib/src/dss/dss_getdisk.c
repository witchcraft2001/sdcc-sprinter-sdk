#include <sprinter/dss.h>

u8 dss_getdisk(void) __naked {
    __asm
        push    ix
        ld      c, #0x02
        rst     #0x10
        pop     ix
        ret
    __endasm;
}
