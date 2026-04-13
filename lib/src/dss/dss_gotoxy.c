#include <sprinter/dss.h>

void dss_gotoxy(u8 x, u8 y) __naked {
    __asm
        ld      e, a
        ld      d, l
        dec     d
        dec     e
        push    ix
        ld      c, #0x52
        rst     #0x10
        pop     ix
        ret
    __endasm;
}
