#include <sprinter/dss.h>

i16 dss_exec(const char *path) __naked {
    __asm
        push    ix
        ld      b, #0           ; subfunction: EXEC0 (auto path handling)
        ld      c, #0x40
        rst     #0x10
        pop     ix
        jr      c, _dss_exec_err
        ld      e, a
        ld      d, #0
        ret
_dss_exec_err:
        ld      de, #0xFFFF
        ret
    __endasm;
}
