#include <sprinter/dss.h>

u16 dss_call(u16 addr) __naked {
    (void)addr;
    __asm
        ; sdcccall(1): addr -> HL
        push    ix          ; save SDCC frame pointer
        call    _dss_call_hl
        ; called code returns value in DE (or HL for u16)
        push    hl
        pop     de          ; return value in DE (sdcccall u16 return)
        pop     ix          ; restore frame pointer
        ret
_dss_call_hl:
        jp      (hl)        ; "call" to (HL): ret from target returns here
    __endasm;
}
