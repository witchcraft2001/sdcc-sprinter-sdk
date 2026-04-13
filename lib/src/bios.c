/**
 * bios.c — BIOS and hardware I/O wrappers for SDCC sdcccall(1)
 *
 * Callee must clean stack params before ret!
 * u8 stack params are packed (1 byte each, no padding).
 */

#include <sprinter/bios.h>

void bios_setpal(u8 index, u8 r, u8 g, u8 b) __naked {
    /* index=A, r=L, g+b on stack (2 bytes). Callee cleans 2 bytes. */
    (void)index; (void)r; (void)g; (void)b;
    __asm
        push    ix
        ld      d, l            ; D = r
        ld      iy, #4
        add     iy, sp
        ld      e, 0 (iy)      ; E = g
        ld      b, 1 (iy)      ; B = b
        ld      c, #0xA1
        rst     #0x08
        pop     ix
        ; clean 2 bytes of stack params
        pop     iy
        inc     sp
        inc     sp
        jp      (iy)
    __endasm;
}

void bios_putpixel(u16 x, u8 y, u8 color) __naked {
    /* x=HL, y+color on stack (2 bytes). Callee cleans 2 bytes. */
    (void)x; (void)y; (void)color;
    __asm
        push    ix
        ld      iy, #4
        add     iy, sp
        ld      d, 0 (iy)      ; D = y
        ld      a, 1 (iy)      ; A = color
        ld      c, #0xA0
        rst     #0x08
        pop     ix
        ; clean 2 bytes
        pop     iy
        inc     sp
        inc     sp
        jp      (iy)
    __endasm;
}

u16 bios_version(void) __naked {
    __asm
        push    ix
        ld      c, #0xEE
        rst     #0x08
        pop     ix
        ld      e, a
        ld      d, b
        ret
    __endasm;
}

u8 bios_board_id(void) __naked {
    __asm
        push    ix
        ld      c, #0xED
        rst     #0x08
        pop     ix
        ret
    __endasm;
}

u8 inp(u16 port) __naked {
    __asm
        ld      c, l
        ld      b, h
        in      a, (c)
        ret
    __endasm;
}

void outp(u16 port, u8 value) __naked {
    /* port=HL, value on stack (1 byte). Callee cleans 1 byte. */
    (void)port; (void)value;
    __asm
        ld      c, l
        ld      b, h
        ld      iy, #2
        add     iy, sp
        ld      a, 0 (iy)
        out     (c), a
        ; clean 1 byte
        pop     iy
        inc     sp
        jp      (iy)
    __endasm;
}
