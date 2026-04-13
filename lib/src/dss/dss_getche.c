#include <sprinter/dss.h>

u16 dss_getche(void) __naked {
    __asm
        push    ix
        ld      c, #0x32
        rst     #0x10
        pop     ix
        ret
    __endasm;
}
