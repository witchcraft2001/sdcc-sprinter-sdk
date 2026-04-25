#include <sprinter/dss.h>

void dss_gotoxy(u8 x, u8 y) __naked {
#if defined(SDCC) && (SDCC < 300)
    __asm
        push    ix
        ld      iy, #4
        add     iy, sp
        ld      e, 0 (iy)
        ld      d, 1 (iy)
        dec     d
        dec     e
        ld      c, #0x52
        rst     #0x10
        pop     ix
        ret
    __endasm;
#else
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
#endif
}
