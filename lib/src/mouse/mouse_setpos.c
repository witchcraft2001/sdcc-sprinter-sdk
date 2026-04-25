#include <sprinter/mouse.h>

void mouse_setpos(u16 x, u16 y) __naked {
    /* x in HL, y in DE (2nd u16) */
#if defined(SDCC) && (SDCC < 300)
    __asm
        push    ix
        ld      iy, #4
        add     iy, sp
        ld      l, 0 (iy)
        ld      h, 1 (iy)
        ld      e, 2 (iy)
        ld      d, 3 (iy)
        ld      c, #0x04
        rst     #0x30
        pop     ix
        ret
    __endasm;
#else
    __asm
        push    ix
        ld      c, #0x04
        rst     #0x30
        pop     ix
        ret
    __endasm;
#endif
}
