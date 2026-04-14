/**
 * 03_graphics — Graphics demo for ZX Sprinter
 *
 * 320x256, 256 colors. BIOS #A6 GRAF palette.
 * Draws palette bars, frame, and gradient.
 */

#include <sprinter.h>

/* Set built-in GRAF palette via BIOS #A6 (B=1) */
static void set_graf_palette(void) {
    __asm
        push    ix
        ld      b, #1
        ld      e, #0
        xor     a
        ld      c, #0xA6
        rst     #0x08
        pop     ix
    __endasm;
}

void main(void) {
    u8 y, c, saved_mode;
    u16 x;

    saved_mode = video_getmode();
    video_setmode(VMODE_320);
    set_graf_palette();

    /* Palette bars: 16 bars of 20px wide, 16px tall */
    for (c = 0; c < 16; c++) {
        for (y = 4; y < 20; y++) {
            for (x = (u16)c * 20; x < (u16)c * 20 + 20; x++) {
                bios_putpixel(x, y, c * 16 + 8);
            }
        }
    }

    /* White frame */
    for (x = 0; x < 320; x++) {
        bios_putpixel(x, 28, 255);
        bios_putpixel(x, 240, 255);
    }
    for (y = 28; y < 241; y++) {
        bios_putpixel(0, y, 255);
        bios_putpixel(319, y, 255);
    }

    /* Diagonal color gradient inside frame */
    for (y = 30; y < 240; y++) {
        for (x = 2; x < 318; x++) {
            bios_putpixel(x, y, (u8)((x + (u16)y) & 255));
        }
    }

    video_safe_porty();
    dss_waitkey();
    video_setmode(saved_mode);
}
