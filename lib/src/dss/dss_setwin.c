#include <sprinter/dss.h>

void dss_setwin(u8 win, u8 page) __naked {
#if defined(SDCC) && (SDCC < 300)
    __asm
        push    ix
        ld      iy, #4
        add     iy, sp
        ld      a, 0 (iy)
        ld      b, 1 (iy)
        ld      c, #0x38
        rst     #0x10
        pop     ix
        ret
    __endasm;
#else
    __asm
        push    ix
        ld      b, l
        ld      c, #0x38
        rst     #0x10
        pop     ix
        ret
    __endasm;
#endif
}
