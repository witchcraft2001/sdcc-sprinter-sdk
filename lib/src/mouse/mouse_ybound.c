#include <sprinter/mouse.h>

void mouse_ybound(u16 min_y, u16 max_y) __naked {
    /* min_y in HL, max_y in DE */
    __asm
        push    ix
        ld      c, #0x06
        rst     #0x30
        pop     ix
        ret
    __endasm;
}
