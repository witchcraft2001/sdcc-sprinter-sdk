#include <sprinter/dss.h>

bool dss_scankey(dss_key_t *key) __naked {
    (void)key;
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
        ld      hl, #1
        pop     ix
        ret
no_key:
        pop     hl
        xor     a
        ld      hl, #0
        pop     ix
        ret
    __endasm;
}
