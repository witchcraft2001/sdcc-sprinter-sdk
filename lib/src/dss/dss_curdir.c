#include <sprinter/dss.h>

u8 dss_curdir(char *buf) __naked {
    (void)buf;
    __asm
        push    ix
        ld      c, #0x1E
        rst     #0x10
        pop     ix
        jr      c, _dss_curdir_err
        xor     a
        ret
_dss_curdir_err:
        ret
    __endasm;
}
