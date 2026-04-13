#include <sprinter/dss.h>

bool dss_kbhit(void) __naked {
    __asm
        push    ix
        ld      c, #0x33
        rst     #0x10
        pop     ix
        or      a
        ret     z
        ld      a, #1
        ret
    __endasm;
}
