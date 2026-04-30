#include <sprinter/dss.h>

void dss_freemem(u8 block) __naked {
    (void)block;
    __asm
        push    ix
        ld      c, #0x3E
        rst     #0x10
        pop     ix
        ret
    __endasm;
}
