/**
 * 03_graphics — Graphics mode example for ZX Sprinter
 *
 * Demonstrates 320x256 graphics mode with palette and pixel drawing.
 * Draws a color gradient, some shapes, and waits for key.
 */

#include <sprinter.h>

/* Draw a filled rectangle using BIOS putpixel */
static void fill_rect(u16 x, u8 y, u16 w, u8 h, u8 color) {
    u16 cx;
    u8 cy;
    for (cy = y; cy < y + h; cy++) {
        for (cx = x; cx < x + w; cx++) {
            bios_putpixel(cx, cy, color);
        }
    }
}

/* Draw a horizontal line */
static void hline(u16 x, u8 y, u16 len, u8 color) {
    u16 i;
    for (i = 0; i < len; i++) {
        bios_putpixel(x + i, y, color);
    }
}

/* Draw a vertical line */
static void vline(u16 x, u8 y, u8 len, u8 color) {
    u8 i;
    for (i = 0; i < len; i++) {
        bios_putpixel(x, y + i, color);
    }
}

void main(void) {
    u8 i, y;
    u16 x;

    /* Switch to 320x256 graphics mode */
    video_setmode(VMODE_320);

    /* Set up a 64-color palette: 4R x 4G x 4B = 64 colors */
    for (i = 0; i < 64; i++) {
        u8 r = (i >> 4) & 3;
        u8 g = (i >> 2) & 3;
        u8 b = i & 3;
        video_setpal(i, r, g, b);
    }

    /* Draw color palette bars at top */
    for (i = 0; i < 64; i++) {
        fill_rect((u16)i * 5, 0, 5, 20, i);
    }

    /* Draw a frame */
    hline(10, 30, 300, 63);     /* white top */
    hline(10, 230, 300, 63);    /* white bottom */
    vline(10, 30, 200, 63);     /* white left */
    vline(309, 30, 200, 63);    /* white right */

    /* Draw gradient pattern inside frame */
    for (y = 40; y < 220; y++) {
        for (x = 20; x < 300; x++) {
            u8 color = (u8)(((x - 20) + (u16)(y - 40)) & 63);
            bios_putpixel(x, y, color);
        }
    }

    /* Wait for key press */
    dss_waitkey();

    /* Restore text mode before exit */
    video_setmode(VMODE_ZX);
}
