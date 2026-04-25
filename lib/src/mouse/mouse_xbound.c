#include <sprinter/mouse.h>

void mouse_xbound(u16 min_x, u16 max_x) __naked {
    /* min_x in HL, max_x in DE */
#if defined(SDCC) && (SDCC < 300)
    __asm
        push    ix
        ld      iy, #4
        add     iy, sp
        ld      l, 0 (iy)
        ld      h, 1 (iy)
        ld      e, 2 (iy)
        ld      d, 3 (iy)
        ld      c, #0x05
        rst     #0x30
        pop     ix
        ret
    __endasm;
#else
    __asm
        push    ix
        ld      c, #0x05
        rst     #0x30
        pop     ix
        ret
    __endasm;
#endif
}
