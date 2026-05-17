/*
 * 29_fade — palette fade demo.
 *
 * Loads a 320x256 256-colour image, programs the palette with
 * video_setpal256_fast(), then runs repeated fade out / fade in cycles
 * via the step-driven fade API. PT3 music keeps playing throughout the
 * fades and a small border-colour pulse demonstrates that user logic
 * still runs between fade_step() calls.
 *
 * Controls:
 *   ESC      - exit
 *   any key  - skip current fade phase
 */

#include <sprinter.h>
#include <sprinter/gfx.h>
#include <sprinter/fade.h>
#include "res.h"

#define MUSIC_PAGES         1
#define FADE_OUT_FRAMES     50      /* 1 second at 50 Hz */
#define FADE_IN_FRAMES      50
#define HOLD_FRAMES         60

/* Drive PT3 + scan the keyboard each frame. Returns 0 normally,
 * 1 when ESC is pressed (quit), 2 when any other key is pressed
 * (skip current phase). */
static u8 frame_work(u8 music_block) {
    dss_key_t key;

    ay_pt3_play(music_block);

    while (dss_scankey(&key)) {
        if (key.ascii == 27) return 1;
        return 2;
    }
    return 0;
}

static u8 run_phase(u8 frames, u8 mode, u8 music_block) {
    fade_state_t fade;
    u8 ev;

    if (!fade_begin(&fade, frames, mode)) return 0;
    while (fade.active) {
        video_vsync();
        ev = frame_work(music_block);
        if (ev == 1) {
            fade_cancel(&fade);
            return 1;
        }
        if (ev == 2) {
            fade_cancel(&fade);
            if (mode == FADE_MODE_OUT)
                fade_out(0);
            else
                fade_in(0);
            break;
        }
        fade_step(&fade);
    }
    return 0;
}

static u8 hold(u8 frames, u8 music_block) {
    u8 i;
    u8 ev;

    for (i = 0; i < frames; i++) {
        video_vsync();
        ev = frame_work(music_block);
        if (ev == 1) return 1;
        if (ev == 2) return 0;
    }
    return 0;
}

static void demo(u8 image_block, u8 music_block) {
    u8 quit;

    /* Initial setup: install palette and draw the picture into both screens. */
    video_setpal256_fast(fade_palette);
    fade_capture_palette(fade_palette);

    gfx_select_resource_block(image_block, FADE_IMG_PAGES);
    gfx_draw_image_page(GFX_SCREEN_0, 0, 0, image_block, &fade_image, GFX_OPAQUE);
    gfx_draw_image_page(GFX_SCREEN_1, 0, 0, image_block, &fade_image, GFX_OPAQUE);

    ay_pt3_init(music_block);
    quit = 0;

    while (!quit) {
        if (hold(HOLD_FRAMES, music_block)) { quit = 1; break; }
        if (run_phase(FADE_OUT_FRAMES, FADE_MODE_OUT, music_block)) { quit = 1; break; }
        if (hold(HOLD_FRAMES / 2, music_block)) { quit = 1; break; }
        if (run_phase(FADE_IN_FRAMES, FADE_MODE_IN, music_block)) { quit = 1; break; }
    }

    ay_pt3_mute(music_block);
}

void main(void) {
    u8 image_block;
    u8 music_block;
    u8 graphics_started;

    image_block = 0xFF;
    music_block = 0xFF;
    graphics_started = 0;

    dss_puts("FADE demo (29_fade)\r\n");
    dss_puts("Loading FADE.GFX...\r\n");

    image_block = dss_getmem_pages(FADE_IMG_PAGES);
    music_block = dss_getmem_pages(MUSIC_PAGES);

    if (image_block != 0xFF && music_block != 0xFF &&
        gfx_load_resource_pages("FADE.GFX", image_block, FADE_IMG_PAGES) > 0 &&
        asset_load_pages("FADE.PT3", music_block, MUSIC_PAGES) > 0) {

        dss_puts("ESC quits, any other key skips current phase.\r\n");
        video_setmode(VMODE_320);
        graphics_started = 1;
        demo(image_block, music_block);
    } else {
        dss_puts("Resource load failed.\r\n");
        dss_waitkey();
    }

    if (music_block != 0xFF) {
        ay_pt3_mute(music_block);
        dss_freemem(music_block);
    }
    if (image_block != 0xFF)
        dss_freemem(image_block);
    if (graphics_started)
        video_setmode(VMODE_TEXT80);
}
