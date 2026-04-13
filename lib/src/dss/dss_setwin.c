#include <sprinter/dss.h>

void dss_setwin(u8 win, u8 page) __naked {
    __asm
        push    ix
        ld      b, l
        ld      c, #0x38
        rst     #0x10
        pop     ix
        ret
    __endasm;
}
