#include <sprinter/dss.h>

u8 dss_getmem(void) __naked {
    __asm
        push    ix
        ld      b, #1           ; allocate 1 page
        jr      _dss_getmem_call
    __endasm;
}

u8 dss_getmem_pages(u8 pages) __naked {
    (void)pages;
    __asm
        push    ix
        or      a
        jr      z, _dss_gm_err
        ld      b, a
_dss_getmem_call:
        ld      c, #0x3D
        rst     #0x10
        pop     ix
        jr      c, _dss_gm_err_ret
        ret
_dss_gm_err:
        pop     ix
_dss_gm_err_ret:
        ld      a, #0xFF
        ret
    __endasm;
}
