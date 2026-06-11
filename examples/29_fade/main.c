/*
 * 29_fade - palette fade demo (self-contained).
 *
 * Demonstrates the ported step-paced fade module. The reference SDK's fade
 * example drives an HRUST-packed image plus PT3 music through the gfx/audio
 * subsystems; those are not part of this SDK, so this version is trimmed to
 * the palette/fade feature itself:
 *
 *   - build a 256-entry gradient palette,
 *   - upload it with video_setpal256_fast() (both hardware palette pages),
 *   - fill the 320x256 screen with one palette index per row (255 bands),
 *   - capture the palette and run fade-out / fade-in cycles with the step
 *     API (fade_begin/fade_step), checking the keyboard each frame.
 *
 * The fade hot path (fade_apply.s) writes the dimmed palette straight into
 * VRAM page #50, so every band dims and brightens together. Press any key
 * to exit.
 */
#include <sprinter.h>
#include <sprinter/fade.h>

#define FADE_FRAMES     48      /* frames per fade-out / fade-in            */
#define CYCLES          3       /* out+in cycles before waiting for a key  */

static video_rgb8_t pal[256];

static void build_palette(void) {
    u16 i;
    for (i = 0; i < 256; i++) {
        pal[i].r = (u8)i;                       /* red ramps up   */
        pal[i].g = (u8)(255 - i);               /* green ramps down */
        pal[i].b = (u8)((i << 1) ^ 0x80);       /* blue zig-zags   */
    }
    pal[0].r = 0; pal[0].g = 0; pal[0].b = 0;   /* index 0 = black */
}

static void fill_bands(void) {
    u16 x;
    u8  y;
    for (y = 0; y < 255; y++) {
        for (x = 0; x < SCREEN_W_320; x++) bios_putpixel(x, y, y);
    }
    for (x = 0; x < SCREEN_W_320; x++) bios_putpixel(x, 255, 255);
    video_safe_porty();
}

/* Run one fade in the step API so the keyboard can interrupt it.
 * Returns 1 if a key was pressed (caller should stop). */
static u8 run_fade(u8 mode, u8 frames) {
    fade_state_t st;
    fade_begin(&st, frames, mode);
    while (st.active) {
        video_vsync();
        fade_step(&st);
        if (dss_kbhit()) {
            fade_cancel(&st);
            video_setpal256_fast(pal);          /* snap back to full palette */
            return 1;
        }
    }
    return 0;
}

void main(void) {
    u8 c;

    video_setmode(VMODE_320);
    build_palette();
    video_setpal256_fast(pal);
    fill_bands();
    fade_capture_palette(pal);

    for (c = 0; c < CYCLES; c++) {
        if (run_fade(FADE_MODE_OUT, FADE_FRAMES)) goto done;
        if (run_fade(FADE_MODE_IN,  FADE_FRAMES)) goto done;
    }

    dss_waitkey();

done:
    video_setmode(VMODE_TEXT80);
}
