#include <sprinter/mouse.h>

void mouse_ybound(u16 min_y, u16 max_y) __naked {
    /* min_y in HL, max_y in DE */
#if defined(SDCC) && (SDCC < 300)
    __asm
        push    ix
        ld      iy, #4
        add     iy, sp
        ld      l, 0 (iy)
        ld      h, 1 (iy)
        ld      e, 2 (iy)
        ld      d, 3 (iy)
        ld      c, #0x06
        rst     #0x30
        pop     ix
        ret
    __endasm;
#else
    __asm
        push    ix
        ld      c, #0x06
        rst     #0x30
        pop     ix
        ret
    __endasm;
#endif
}
