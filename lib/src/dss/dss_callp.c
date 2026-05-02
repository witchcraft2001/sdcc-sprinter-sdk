#include <sprinter/dss.h>

u16 dss_callp(u16 addr, u16 param) __naked {
    (void)addr; (void)param;
    __asm
        ; sdcccall(1): addr -> HL, param -> DE
        push    ix          ; save SDCC frame pointer
        ex      de, hl      ; HL = param, DE = target address
        ld      bc, #_dss_callp_done
        ; called SDCC code returns u16 in DE
        push    bc          ; return address for target
        push    de          ; target address
        ret                 ; jump to target with HL = param
_dss_callp_done:
        pop     ix          ; restore frame pointer
        ret
    __endasm;
}
