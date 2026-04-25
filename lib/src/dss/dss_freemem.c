#include <sprinter/dss.h>

void dss_freemem(u8 page) __naked {
#if defined(SDCC) && (SDCC < 300)
    __asm
        push    ix
        ld      iy, #4
        add     iy, sp
        ld      a, 0 (iy)
        ld      c, #0x3E
        rst     #0x10
        pop     ix
        ret
    __endasm;
#else
    __asm
        push    ix
        ld      c, #0x3E
        rst     #0x10
        pop     ix
        ret
    __endasm;
#endif
}
