#include <sprinter/dss.h>

u8 dss_delete(const char *path) __naked {
    __asm
        push    ix
        ld      c, #0x0E
        rst     #0x10
        pop     ix
        jr      c, _dss_del_err
        xor     a
        ret
_dss_del_err:
        ret
    __endasm;
}
