#include <sprinter/mouse.h>

void mouse_hide(void) __naked {
    __asm
        push    ix
        ld      c, #0x02
        rst     #0x30
        pop     ix
        ret
    __endasm;
}
