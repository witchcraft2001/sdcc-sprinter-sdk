#include <sprinter/dss.h>

bool dss_kbhit(void) __naked {
#if defined(SDCC) && (SDCC < 300)
    __asm
        push    ix
        ld      c, #0x33
        rst     #0x10
        pop     ix
        ld      hl, #0x0000
        or      a
        ret     z
        inc     l
        ret
    __endasm;
#else
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
#endif
}
