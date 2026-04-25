#include <sprinter/dss.h>

void dss_setdisk(u8 disk) __naked {
#if defined(SDCC) && (SDCC < 300)
    __asm
        push    ix
        ld      iy, #4
        add     iy, sp
        ld      a, 0 (iy)
        ld      c, #0x01
        rst     #0x10
        pop     ix
        ret
    __endasm;
#else
    __asm
        push    ix
        ld      c, #0x01
        rst     #0x10
        pop     ix
        ret
    __endasm;
#endif
}
