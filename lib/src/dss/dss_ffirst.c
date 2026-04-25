#include <sprinter/dss.h>

i8 dss_ffirst(const char *pattern, dss_find_t *result, u8 attr) __naked {
    (void)pattern; (void)result; (void)attr;
#if defined(SDCC) && (SDCC < 300)
    __asm
        push    ix
        ld      iy, #4
        add     iy, sp
        ld      l, 0 (iy)
        ld      h, 1 (iy)
        ld      e, 2 (iy)
        ld      d, 3 (iy)
        ld      a, 4 (iy)
        ld      b, #1
        ld      c, #0x19
        rst     #0x10
        pop     ix
        jr      c, _dss_ff_err_290
        ld      hl, #0x0000
        ret
_dss_ff_err_290:
        ld      hl, #0xFFFF
        ret
    __endasm;
#else
    __asm
        push    ix
        ld      iy, #4
        add     iy, sp
        ld      a, 0 (iy)      ; attr
        ld      b, #1
        ld      c, #0x19
        rst     #0x10
        pop     ix
        jr      c, _dss_ff_err
        xor     a
        pop     iy
        inc     sp
        jp      (iy)
_dss_ff_err:
        ld      a, #0xFF
        pop     iy
        inc     sp
        jp      (iy)
    __endasm;
#endif
}
