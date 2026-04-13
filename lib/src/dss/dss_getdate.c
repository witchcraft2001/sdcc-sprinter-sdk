#include <sprinter/dss.h>

void dss_getdate(dss_date_t *d) __naked {
    __asm
        push    ix
        push    hl
        ld      c, #0x21
        rst     #0x10
        push    ix
        pop     bc
        pop     hl
        ld      (hl), c
        inc     hl
        ld      (hl), b
        inc     hl
        ld      (hl), d
        inc     hl
        ld      (hl), e
        pop     ix
        ret
    __endasm;
}
