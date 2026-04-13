#include <sprinter/dss.h>

u8 dss_getmem(void) __naked {
    __asm
        push    ix
        ld      c, #0x3D
        rst     #0x10
        pop     ix
        jr      c, _dss_gm_err
        ret
_dss_gm_err:
        ld      a, #0xFF
        ret
    __endasm;
}
