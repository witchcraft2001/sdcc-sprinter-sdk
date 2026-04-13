#include <sprinter/mouse.h>

void mouse_stat(mouse_state_t *state) __naked {
    /* state in HL */
    __asm
        push    ix
        push    hl
        ld      c, #0x03
        rst     #0x30
        ; A=buttons, HL=X, DE=Y
        ex      (sp), hl        ; stack=X, HL=state ptr
        ld      (hl), a         ; buttons
        inc     hl
        pop     bc              ; BC=X
        ld      (hl), c
        inc     hl
        ld      (hl), b
        inc     hl
        ld      (hl), e         ; Y low
        inc     hl
        ld      (hl), d         ; Y high
        pop     ix
        ret
    __endasm;
}
