#include <sprinter/mouse.h>

void mouse_xbound(u16 min_x, u16 max_x) __naked {
    /* min_x in HL, max_x in DE */
    __asm
        push    ix
        ld      c, #0x05
        rst     #0x30
        pop     ix
        ret
    __endasm;
}
