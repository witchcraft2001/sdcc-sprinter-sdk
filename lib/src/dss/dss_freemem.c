#include <sprinter/dss.h>

void dss_freemem(u8 page) __naked {
    __asm
        push    ix
        ld      c, #0x3E
        rst     #0x10
        pop     ix
        ret
    __endasm;
}
