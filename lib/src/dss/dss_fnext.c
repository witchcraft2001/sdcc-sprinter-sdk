#include <sprinter/dss.h>

i8 dss_fnext(dss_find_t *result) __naked {
    __asm
        push    ix
        ex      de, hl
        ld      c, #0x1A
        rst     #0x10
        pop     ix
        jr      c, _dss_fn_err
        xor     a
        ret
_dss_fn_err:
        ld      a, #0xFF
        ret
    __endasm;
}
