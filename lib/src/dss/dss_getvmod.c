#include <sprinter/dss.h>

void dss_getvmod(u8 *mode, u8 *page) __naked {
    (void)mode; (void)page;
    __asm
        ; sdcccall(1): mode -> HL, page -> DE
        push    ix
        push    hl              ; save mode ptr
        push    de              ; save page ptr
        ld      c, #0x51
        rst     #0x10
        ; A = mode, B = page
        pop     hl              ; HL = page ptr
        ld      (hl), b
        pop     hl              ; HL = mode ptr
        ld      (hl), a
        pop     ix
        ret
    __endasm;
}
