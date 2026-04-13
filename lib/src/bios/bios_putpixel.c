#include <sprinter/bios.h>
#include <sprinter/ports.h>

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
