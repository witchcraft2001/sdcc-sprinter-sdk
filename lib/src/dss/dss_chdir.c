#include <sprinter/dss.h>

u8 dss_chdir(const char *path) __naked {
    __asm
        push    ix
        ld      c, #0x1D
        rst     #0x10
        pop     ix
        jr      c, _dss_cd_err
        xor     a
        ret
_dss_cd_err:
        ret
    __endasm;
}
