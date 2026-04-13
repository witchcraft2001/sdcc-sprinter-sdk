#include <sprinter/dss.h>

i16 dss_creat(const char *path) __naked {
    __asm
        push    ix
        ld      a, #0x20        ; attribute: archive
        ld      c, #0x0A
        rst     #0x10
        pop     ix
        jr      c, _dss_creat_err
        ld      e, a
        ld      d, #0
        ret
_dss_creat_err:
        ld      de, #0xFFFF
        ret
    __endasm;
}
