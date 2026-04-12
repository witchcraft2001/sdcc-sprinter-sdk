/**
 * bios.c — BIOS and hardware I/O wrappers for SDCC
 */

#include <sprinter/bios.h>

void bios_setpal(u8 index, u8 r, u8 g, u8 b) __naked {
    __asm
        ld      hl, #2
        add     hl, sp
        ld      a, (hl)         ; index
        inc     hl
        inc     hl
        ld      d, (hl)         ; r
        inc     hl
        inc     hl
        ld      e, (hl)         ; g
        inc     hl
        inc     hl
        ld      b, (hl)         ; b
        ; Pack RGB: A=index, D=R(0-3), E=G(0-3), B=B(0-3)
        ld      c, #0xA1        ; BIOS.SetPal
        rst     #0x08
        ret
    __endasm;
}

void bios_putpixel(u16 x, u8 y, u8 color) __naked {
    __asm
        ld      hl, #2
        add     hl, sp
        ld      e, (hl)         ; x low
        inc     hl
        ld      d, (hl)         ; x high
        inc     hl
        ld      b, (hl)         ; y
        inc     hl
        inc     hl
        ld      a, (hl)         ; color
        push    de
        pop     hl              ; HL = x
        ld      d, b            ; D = y
        ld      c, #0xA0        ; BIOS.PutPixel
        rst     #0x08
        ret
    __endasm;
}

u16 bios_version(void) __naked {
    __asm
        ld      c, #0xEE        ; BIOS.Version
        rst     #0x08
        ld      l, a
        ld      h, b
        ret
    __endasm;
}

u8 bios_board_id(void) __naked {
    __asm
        ld      c, #0xED        ; BIOS.BoardID
        rst     #0x08
        ld      l, a
        ret
    __endasm;
}

u8 inp(u16 port) __z88dk_fastcall __naked {
    __asm
        ld      c, l
        ld      b, h
        in      a, (c)
        ld      l, a
        ret
    __endasm;
}

void outp(u16 port, u8 value) __naked {
    __asm
        ld      hl, #2
        add     hl, sp
        ld      c, (hl)         ; port low
        inc     hl
        ld      b, (hl)         ; port high
        inc     hl
        ld      a, (hl)         ; value
        out     (c), a
        ret
    __endasm;
}
