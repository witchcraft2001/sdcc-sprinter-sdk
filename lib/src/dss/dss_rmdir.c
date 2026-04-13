#include <sprinter/dss.h>

u8 dss_rmdir(const char *path) __naked {
    (void)path;
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
}
