#include <sprinter/mouse.h>

void mouse_show(void) __naked {
    __asm
        push    ix
        ld      c, #0x01
        rst     #0x30
        pop     ix
        ret
    __endasm;
}
