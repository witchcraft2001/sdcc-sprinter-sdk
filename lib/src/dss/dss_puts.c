#include <sprinter/dss.h>

void dss_puts(const char *str) __naked {
#if defined(SDCC) && (SDCC < 300)
    __asm
        push    ix
        ld      iy, #4
        add     iy, sp
        ld      l, 0 (iy)
        ld      h, 1 (iy)
        ld      c, #0x5C
        rst     #0x10
        pop     ix
        ret
    __endasm;
#else
    __asm
        push    ix
        ld      c, #0x5C
        rst     #0x10
        pop     ix
        ret
    __endasm;
#endif
}
