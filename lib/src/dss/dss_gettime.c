#include <sprinter/dss.h>

void dss_gettime(dss_time_t *t) __naked {
#if defined(SDCC) && (SDCC < 300)
    __asm
        push    ix
        push    hl
        ld      c, #0x21
        rst     #0x10
        ex      (sp), hl
        pop     de
        ld      (hl), e
        inc     hl
        ld      (hl), d
        inc     hl
        ld      (hl), #0
        inc     hl
        ld      (hl), b
        pop     ix
        ret
    __endasm;
#else
    __asm
        push    ix
        push    hl
        ld      c, #0x21
        rst     #0x10
        ex      de, hl
        pop     hl
        ld      (hl), e
        inc     hl
        ld      (hl), d
        inc     hl
        ld      (hl), #0
        inc     hl
        ld      (hl), b
        pop     ix
        ret
    __endasm;
#endif
}
