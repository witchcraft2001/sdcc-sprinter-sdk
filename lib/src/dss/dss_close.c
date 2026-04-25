#include <sprinter/dss.h>

u8 dss_close(u8 fd) __naked {
#if defined(SDCC) && (SDCC < 300)
    __asm
        push    ix
        ld      iy, #4
        add     iy, sp
        ld      a, 0 (iy)
        ld      c, #0x12
        rst     #0x10
        pop     ix
        ld      l, a
        ld      h, #0x00
        ret
    __endasm;
#else
    __asm
        push    ix
        ld      c, #0x12
        rst     #0x10
        pop     ix
        ret
    __endasm;
#endif
}
