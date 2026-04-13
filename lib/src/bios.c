/**
 * bios.c — BIOS, hardware I/O and graphics for SDCC sdcccall(1)
 *
 * Palette: PORT_Y (#89) protocol: 0xC0, index, R, G, B (0-63 each)
 * Pixels: PORT_Y for row select, VRAM page #50 mapped to WIN3,
 *         pixel at (WIN3_BASE + 0x20 + x) for screen A page 0.
 *         0x20 offset confirmed by zx-sprinter-sdk _clear_screen.
 */

#include <sprinter/bios.h>
#include <sprinter/ports.h>

void bios_setpal(u8 index, u8 r, u8 g, u8 b) __naked {
    /* index=A, r=L, g+b on stack (2 bytes). Callee cleans 2. */
    (void)index; (void)r; (void)g; (void)b;
    __asm
        push    ix
        ld      d, a            ; D = index
        ld      e, l            ; E = r
        ld      iy, #4
        add     iy, sp

        ld      a, #0xC0
        out     (#0x89), a      ; palette command
        ld      a, d
        out     (#0x89), a      ; index
        ld      a, e
        out     (#0x89), a      ; R
        ld      a, 0 (iy)
        out     (#0x89), a      ; G
        ld      a, 1 (iy)
        out     (#0x89), a      ; B

        pop     ix
        pop     iy
        inc     sp
        inc     sp
        jp      (iy)
    __endasm;
}

void bios_putpixel(u16 x, u8 y, u8 color) __naked {
    /* x=HL, y+color on stack (2 bytes). Callee cleans 2.
     *
     * Uses PORT_Y (#89) for row selection, VRAM page #50 in WIN3.
     * Screen A pixel address: WIN3_BASE(0xC000) + 0x20 + x
     */
    (void)x; (void)y; (void)color;
    __asm
        push    ix

        ld      iy, #4
        add     iy, sp
        ld      d, 0 (iy)      ; D = y
        ld      e, 1 (iy)      ; E = color

        ; Save WIN3 page
        in      a, (#0xE2)
        push    af

        ; Map VRAM page #50 to WIN3
        ld      a, #0x50
        out     (#0xE2), a

        ; Set PORT_Y = y (row select)
        ld      a, d
        out     (#0x89), a

        ; Calculate pixel address: 0xC000 + x
        ld      bc, #0xC000
        add     hl, bc          ; HL = 0xC000 + x

        ; Write pixel
        ld      (hl), e         ; color

        ; NOTE: don't reset PORT_Y here — caller should call
        ; video_safe_porty() after all drawing is done.

        ; Restore WIN3 page
        pop     af
        out     (#0xE2), a

        pop     ix
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
    (void)port; (void)value;
    __asm
        ld      c, l
        ld      b, h
        ld      iy, #2
        add     iy, sp
        ld      a, 0 (iy)
        out     (c), a
        pop     iy
        inc     sp
        jp      (iy)
    __endasm;
}
