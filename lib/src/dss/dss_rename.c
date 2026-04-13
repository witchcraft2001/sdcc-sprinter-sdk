#include <sprinter/dss.h>

u8 dss_rename(const char *oldpath, const char *newpath) __naked {
    __asm
        push    ix
        ld      c, #0x10
        rst     #0x10
        pop     ix
        jr      c, _dss_ren_err
        xor     a
        ret
_dss_ren_err:
        ret
    __endasm;
}
