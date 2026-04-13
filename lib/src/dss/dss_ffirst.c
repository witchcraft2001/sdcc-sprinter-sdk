#include <sprinter/dss.h>

i8 dss_ffirst(const char *pattern, dss_find_t *result, u8 attr) __naked {
    (void)pattern; (void)result; (void)attr;
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
}
