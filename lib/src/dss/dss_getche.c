#include <sprinter/dss.h>

u16 dss_getche(void) __naked {
#if defined(SDCC) && (SDCC < 300)
    __asm
        push    ix
        ld      c, #0x32
        rst     #0x10
        pop     ix
        ld      l, a
        ld      h, #0
        ret
    __endasm;
#else
    __asm
        push    ix
        ld      c, #0x32
        rst     #0x10
        pop     ix
        ret
    __endasm;
#endif
}
