#include <sprinter/mouse.h>

void mouse_setpos(u16 x, u16 y) __naked {
    /* x in HL, y in DE (2nd u16) */
    __asm
        push    ix
        ld      c, #0x04
        rst     #0x30
        pop     ix
        ret
    __endasm;
}
