#include <sprinter/dss.h>

void dss_putchar(u8 ch) __naked {
#if defined(SDCC) && (SDCC < 300)
    __asm
        push    ix
        ld      iy, #4
        add     iy, sp
        ld      a, 0 (iy)
        cp      #0x0A
        jr      nz, _dss_pc_out_290
        push    af
        ld      a, #0x0D
        ld      c, #0x5B
        rst     #0x10
        pop     af
_dss_pc_out_290:
        ld      c, #0x5B
        rst     #0x10
        pop     ix
        ret
    __endasm;
#else
    __asm
        push    ix
        cp      #0x0A
        jr      nz, _dss_pc_out
        push    af
        ld      a, #0x0D
        ld      c, #0x5B
        rst     #0x10
        pop     af
_dss_pc_out:
        ld      c, #0x5B
        rst     #0x10
        pop     ix
        ret
    __endasm;
#endif
}
