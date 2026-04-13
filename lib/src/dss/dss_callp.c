#include <sprinter/dss.h>

u16 dss_callp(u16 addr, u16 param) __naked {
    (void)addr; (void)param;
    __asm
        ; sdcccall(1): addr -> HL, param -> DE
        push    ix          ; save SDCC frame pointer
        push    de          ; pass param on stack for called code
        call    _dss_callp_hl
        pop     bc          ; clean param from stack
        push    hl
        pop     de          ; return value in DE
        pop     ix          ; restore frame pointer
        ret
_dss_callp_hl:
        jp      (hl)
    __endasm;
}
