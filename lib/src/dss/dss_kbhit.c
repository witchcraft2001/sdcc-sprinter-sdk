#include <sprinter/dss.h>

bool dss_kbhit(void) __naked {
    __asm
        push    ix
        ld      c, #0x33
        rst     #0x10
        pop     ix
        ld      hl, #0
        or      a
        ret     z
        inc     l
        ret
    __endasm;
}
