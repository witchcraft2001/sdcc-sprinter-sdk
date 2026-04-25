#include <sprinter/dss.h>

u8 dss_rmdir(const char *path) __naked {
    (void)path;
#if defined(SDCC) && (SDCC < 300)
    __asm
        push    ix
        ld      iy, #4
        add     iy, sp
        ld      l, 0 (iy)
        ld      h, 1 (iy)
        ld      c, #0x1C
        rst     #0x10
        pop     ix
        jr      c, _dss_rmdir_err_290
        ld      hl, #0x0000
        ret
_dss_rmdir_err_290:
        ld      l, a
        ld      h, #0x00
        ret
    __endasm;
#else
    __asm
        push    ix
        ld      c, #0x1C
        rst     #0x10
        pop     ix
        jr      c, _dss_rmdir_err
        xor     a
        ret
_dss_rmdir_err:
        ret
    __endasm;
#endif
}
