#include <sprinter/dss.h>

bool dss_scankey(dss_key_t *key) __naked {
    __asm
        push    ix
        push    hl
        ld      c, #0x31
        rst     #0x10
        jr      z, no_key
        pop     hl
        ld      (hl), a
        inc     hl
        ld      (hl), d
        inc     hl
        ld      (hl), b
        inc     hl
        ld      (hl), c
        ld      a, #1
        pop     ix
        ret
no_key:
        pop     hl
        xor     a
        pop     ix
        ret
    __endasm;
}
