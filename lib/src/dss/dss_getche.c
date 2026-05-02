#include <sprinter/dss.h>

u16 dss_getche(void) __naked {
    __asm
        push    ix
        ld      c, #0x32
        rst     #0x10
        pop     ix
        ld      l, a
        ld      h, #0
        ret
    __endasm;
}
