#include <sprinter/dss.h>

void dss_clrscr(void) __naked {
    __asm
        push    ix
        ld      de, #0x0000
        ld      hl, #0x2050
        ld      bc, #0x0756
        ld      a, #0x20
        rst     #0x10
        ld      de, #0x0000
        ld      c, #0x52
        rst     #0x10
        pop     ix
        ret
    __endasm;
}
