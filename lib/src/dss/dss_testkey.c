#include <sprinter/dss.h>

bool dss_testkey(dss_key_t *key) __naked {
#if defined(SDCC) && (SDCC < 300)
    __asm
        push    ix
        ld      iy, #4
        add     iy, sp
        ld      l, 0 (iy)
        ld      h, 1 (iy)
        push    hl
        ld      c, #0x37
        rst     #0x10
        jr      z, no_key_290
        pop     hl
        ld      (hl), a
        inc     hl
        ld      (hl), d
        inc     hl
        ld      (hl), b
        inc     hl
        ld      (hl), c
        pop     ix
        ld      hl, #0x0001
        ret
no_key_290:
        pop     hl
        pop     ix
        ld      hl, #0x0000
        ret
    __endasm;
#else
    __asm
        push    ix
        push    hl
        ld      c, #0x37
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
#endif
}
