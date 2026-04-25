#include <sprinter/dss.h>

void dss_exit(u8 code) __naked {
#if defined(SDCC) && (SDCC < 300)
    __asm
        push    ix
        ld      iy, #4
        add     iy, sp
        ld      a, 0 (iy)
        ld      b, a
        ld      c, #0x41
        rst     #0x10
        pop     ix
        ret
    __endasm;
#else
    __asm
        ld      b, a
        ld      c, #0x41
        rst     #0x10
        ret
    __endasm;
#endif
}
