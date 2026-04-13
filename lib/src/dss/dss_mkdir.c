#include <sprinter/dss.h>

u8 dss_mkdir(const char *path) __naked {
    (void)path;
    __asm
        push    ix
        ld      c, #0x1B
        rst     #0x10
        pop     ix
        jr      c, _dss_mkdir_err
        xor     a
        ret
_dss_mkdir_err:
        ret
    __endasm;
}
