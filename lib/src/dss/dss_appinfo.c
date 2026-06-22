#include <sprinter/dss.h>

i8 dss_appinfo(u8 subfunc, char *buf) __naked {
    (void)subfunc; (void)buf;
    __asm
        ; sdcccall(1): subfunc -> A, buf -> DE
        push    ix
        ex      de, hl          ; HL = buf (DSS 0x47 expects buffer in HL)
        ld      b, a            ; B = subfunc
        ld      c, #0x47
        rst     #0x10
        pop     ix
        jr      c, _dss_ai_err
        xor     a
        ret
_dss_ai_err:
        ld      a, #0xFF
        ret
    __endasm;
}
