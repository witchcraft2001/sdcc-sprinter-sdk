#include <sprinter/dss.h>

i16 dss_exec(const char *path) __naked {
    (void)path;
#if defined(SDCC) && (SDCC < 300)
    __asm
        push    ix
        ld      iy, #4
        add     iy, sp
        ld      l, 0 (iy)
        ld      h, 1 (iy)
        ld      b, #0           ; subfunction: EXEC0 (auto path handling)
        ld      c, #0x40
        rst     #0x10
        pop     ix
        jr      c, _dss_exec_err_290
        ld      l, a
        ld      h, #0
        ret
_dss_exec_err_290:
        ld      hl, #0xFFFF
        ret
    __endasm;
#else
    __asm
        push    ix
        ld      b, #0           ; subfunction: EXEC0 (auto path handling)
        ld      c, #0x40
        rst     #0x10
        pop     ix
        jr      c, _dss_exec_err
        ld      e, a
        ld      d, #0
        ret
_dss_exec_err:
        ld      de, #0xFFFF
        ret
    __endasm;
#endif
}
