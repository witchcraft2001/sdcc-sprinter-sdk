/**
 * mouse.c — Mouse driver wrappers for SDCC sdcccall(1)
 *
 * Mouse API via RST #30 — save/restore IX around all calls.
 */

#include <sprinter/mouse.h>

u8 mouse_init(void) __naked {
    __asm
        push    ix
        ld      c, #0x00
        rst     #0x30
        pop     ix
        ret
    __endasm;
}

void mouse_show(void) __naked {
    __asm
        push    ix
        ld      c, #0x01
        rst     #0x30
        pop     ix
        ret
    __endasm;
}

void mouse_hide(void) __naked {
    __asm
        push    ix
        ld      c, #0x02
        rst     #0x30
        pop     ix
        ret
    __endasm;
}

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

void mouse_setpos(u16 x, u16 y) __naked {
    /* x in HL, y in DE (2nd u16) */
    __asm
        push    ix
        ld      c, #0x04
        rst     #0x30
        pop     ix
        ret
    __endasm;
}

void mouse_xbound(u16 min_x, u16 max_x) __naked {
    /* min_x in HL, max_x in DE */
    __asm
        push    ix
        ld      c, #0x05
        rst     #0x30
        pop     ix
        ret
    __endasm;
}

void mouse_ybound(u16 min_y, u16 max_y) __naked {
    /* min_y in HL, max_y in DE */
    __asm
        push    ix
        ld      c, #0x06
        rst     #0x30
        pop     ix
        ret
    __endasm;
}
