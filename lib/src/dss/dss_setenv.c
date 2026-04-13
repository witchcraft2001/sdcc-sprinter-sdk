#include <sprinter/dss.h>

i8 dss_setenv(const char *namevalue) __naked {
    (void)namevalue;
    __asm
        ; sdcccall(1): namevalue -> HL (format: "NAME=VALUE")
        push    ix
        ld      b, #2           ; subfunction: SETENV
        ld      c, #0x46
        rst     #0x10
        pop     ix
        jr      c, _dss_se_err
        xor     a
        ret
_dss_se_err:
        ld      a, #0xFF
        ret
    __endasm;
}
