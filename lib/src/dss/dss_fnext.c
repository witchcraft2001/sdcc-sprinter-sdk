#include <sprinter/dss.h>

i8 dss_fnext(dss_find_t *result) __naked {
#if defined(SDCC) && (SDCC < 300)
    __asm
        push    ix
        ld      iy, #4
        add     iy, sp
        ld      e, 0 (iy)
        ld      d, 1 (iy)
        ld      c, #0x1A
        rst     #0x10
        pop     ix
        jr      c, _dss_fn_err_290
        ld      hl, #0x0000
        ret
_dss_fn_err_290:
        ld      hl, #0xFFFF
        ret
    __endasm;
#else
    __asm
        push    ix
        ex      de, hl
        ld      c, #0x1A
        rst     #0x10
        pop     ix
        jr      c, _dss_fn_err
        xor     a
        ret
_dss_fn_err:
        ld      a, #0xFF
        ret
    __endasm;
#endif
}
