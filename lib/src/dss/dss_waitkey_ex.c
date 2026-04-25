#include <sprinter/dss.h>

void dss_waitkey_ex(dss_key_t *key) __naked {
#if defined(SDCC) && (SDCC < 300)
    __asm
        push    ix
        ld      iy, #4
        add     iy, sp
        ld      l, 0 (iy)
        ld      h, 1 (iy)
        push    hl
        ld      c, #0x30
        rst     #0x10
        pop     hl
        ld      (hl), a
        inc     hl
        ld      (hl), d
        inc     hl
        ld      (hl), b
        inc     hl
        ld      (hl), c
        pop     ix
        ret
    __endasm;
#else
    __asm
        push    ix
        push    hl
        ld      c, #0x30
        rst     #0x10
        pop     hl
        ld      (hl), a
        inc     hl
        ld      (hl), d
        inc     hl
        ld      (hl), b
        inc     hl
        ld      (hl), c
        pop     ix
        ret
    __endasm;
#endif
}
