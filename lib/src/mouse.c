/**
 * mouse.c — Mouse driver wrappers for SDCC
 *
 * Mouse API via RST #30 vector.
 */

#include <sprinter/mouse.h>

u8 mouse_init(void) __naked {
    __asm
        ld      c, #0x00        ; MS_INIT
        rst     #0x30
        ld      l, a            ; number of buttons
        ret
    __endasm;
}

void mouse_show(void) __naked {
    __asm
        ld      c, #0x01        ; MS_SHOW
        rst     #0x30
        ret
    __endasm;
}

void mouse_hide(void) __naked {
    __asm
        ld      c, #0x02        ; MS_HIDE
        rst     #0x30
        ret
    __endasm;
}

void mouse_stat(mouse_state_t *state) __z88dk_fastcall __naked {
    __asm
        push    hl              ; save state pointer
        ld      c, #0x03        ; MS_STAT
        rst     #0x30
        ; Returns: A = buttons, HL = X, DE = Y
        pop     bc              ; BC = state pointer
        push    bc
        push    hl              ; save X
        push    de              ; save Y
        pop     hl              ; HL = Y
        ex      (sp), hl        ; stack=Y, HL=X
        ex      de, hl          ; DE = X
        pop     hl              ; HL = Y
        push    hl              ; re-save Y
        ; Now: A=buttons, DE=X, on stack=Y
        pop     hl              ; HL = Y
        push    hl              ; save Y again
        ; Write to struct via BC pointer
        ld      h, b
        ld      l, c            ; HL = state pointer
        ld      (hl), a         ; buttons
        inc     hl
        ld      (hl), e         ; x low
        inc     hl
        ld      (hl), d         ; x high
        inc     hl
        pop     de              ; DE = Y
        ld      (hl), e         ; y low
        inc     hl
        ld      (hl), d         ; y high
        ret
    __endasm;
}

void mouse_setpos(u16 x, u16 y) __naked {
    __asm
        ld      hl, #2
        add     hl, sp
        ld      e, (hl)         ; x low
        inc     hl
        ld      d, (hl)         ; x high
        inc     hl
        push    de              ; save x
        ld      e, (hl)         ; y low
        inc     hl
        ld      d, (hl)         ; y high
        pop     hl              ; HL = x
        ; HL = x, DE = y
        ld      c, #0x04        ; MS_SETPOS
        rst     #0x30
        ret
    __endasm;
}

void mouse_xbound(u16 min_x, u16 max_x) __naked {
    __asm
        ld      hl, #2
        add     hl, sp
        ld      e, (hl)         ; min_x low
        inc     hl
        ld      d, (hl)         ; min_x high
        inc     hl
        push    de              ; save min
        ld      e, (hl)         ; max_x low
        inc     hl
        ld      d, (hl)         ; max_x high
        pop     hl              ; HL = min_x
        ; HL = min, DE = max
        ld      c, #0x05        ; MS_XBOUND
        rst     #0x30
        ret
    __endasm;
}

void mouse_ybound(u16 min_y, u16 max_y) __naked {
    __asm
        ld      hl, #2
        add     hl, sp
        ld      e, (hl)         ; min_y low
        inc     hl
        ld      d, (hl)         ; min_y high
        inc     hl
        push    de              ; save min
        ld      e, (hl)         ; max_y low
        inc     hl
        ld      d, (hl)         ; max_y high
        pop     hl              ; HL = min_y
        ; HL = min, DE = max
        ld      c, #0x06        ; MS_YBOUND
        rst     #0x30
        ret
    __endasm;
}
