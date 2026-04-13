#include <sprinter/dss.h>

i8 dss_getenv(const char *name, char *buf) __naked {
    (void)name; (void)buf;
    __asm
        ; sdcccall(1): name -> HL, buf -> DE
        push    ix
        ld      b, #1           ; subfunction: GETENV
        ld      c, #0x46
        rst     #0x10
        pop     ix
        jr      c, _dss_ge_err
        xor     a
        ret
_dss_ge_err:
        ld      a, #0xFF
        ret
    __endasm;
}
