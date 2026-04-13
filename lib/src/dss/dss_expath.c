#include <sprinter/dss.h>

i8 dss_expath(const char *path, char *buf, u8 subfunc) __naked {
    (void)path; (void)buf; (void)subfunc;
    __asm
        ; sdcccall(1): path -> HL, buf -> DE, subfunc -> stack
        push    ix
        ld      iy, #4
        add     iy, sp
        ld      b, 0 (iy)      ; B = subfunc from stack
        ld      c, #0x45
        rst     #0x10
        pop     ix
        jr      c, _dss_ep_err
        xor     a
        ; callee cleans 1 stack byte
        pop     iy
        inc     sp
        jp      (iy)
_dss_ep_err:
        ld      a, #0xFF
        pop     iy
        inc     sp
        jp      (iy)
    __endasm;
}
