#include <sprinter/fade.h>
#include <sprinter/video.h>
#include <sprinter/ay.h>
#include <sprinter/bios.h>

/*
 * Step-paced palette fade engine.
 *
 * fade_capture_palette() copies the source RGB6 palette into
 * fade_source. Each fade_step() applies the current step's component
 * LUT and pushes one bios_setpal() per colour (which internally
 * updates both hardware palette pages on the stack-backed path that
 * has been verified to produce correct colours).
 *
 * fade_step() does NOT wait for vsync and does NOT play music - the
 * caller drives the frame loop. The blocking helpers (fade_in/out
 * and fade_in/out_music) wrap fade_step() with vsync + optional PT3.
 */

static video_rgb6_t fade_source[256];
static video_rgb6_t fade_temp[256];
static u8 fade_lut[64];
static u8 fade_lut_step;
static u8 fade_lut_ready;

static void fade_build_lut(u8 step) {
    u8  c;
    u16 t;

    if (fade_lut_ready && step == fade_lut_step) return;
    fade_lut_ready = 1;
    fade_lut_step = step;
    for (c = 0; c < 64; c++) {
        t = (u16)c * (u16)step;
        fade_lut[c] = (u8)(t >> 5);     /* divide by FADE_STEPS (=32) */
    }
}

static void fade_apply(u8 step) {
    u8 i;
    u8 done;

    fade_build_lut(step);

    i = 0;
    done = 0;
    while (!done) {
        fade_temp[i].r = fade_lut[fade_source[i].r];
        fade_temp[i].g = fade_lut[fade_source[i].g];
        fade_temp[i].b = fade_lut[fade_source[i].b];
        bios_setpal(i, fade_temp[i].r, fade_temp[i].g, fade_temp[i].b);
        i++;
        if (i == 0) done = 1;
    }
}

void fade_capture_palette(const video_rgb6_t *palette) {
    u16 i;
    const u8 *src;
    u8 *dst;

    src = (const u8 *)palette;
    dst = (u8 *)fade_source;
    for (i = 0; i < sizeof(fade_source); i++) {
        dst[i] = src[i];
    }
    fade_lut_ready = 0;
}

u8 fade_begin(fade_state_t *state, u8 duration_frames, u8 mode) {
    state->mode = mode;
    state->duration_frames = duration_frames;
    state->frame = 0;
    state->step = 0;
    state->accumulator = 0;

    if (duration_frames == 0) {
        if (mode == FADE_MODE_IN) {
            fade_apply(FADE_STEPS);
            state->step = FADE_STEPS;
        } else {
            fade_apply(0);
            state->step = 0;
        }
        state->active = 0;
        return 0;
    }

    state->active = 1;
    return 1;
}

u8 fade_step(fade_state_t *state) {
    u8 progress;
    u8 palette_step;

    if (!state->active) return 0;

    state->accumulator = (u16)(state->accumulator + FADE_STEPS);
    progress = (u8)(state->accumulator / state->duration_frames);
    if (progress > FADE_STEPS) progress = FADE_STEPS;

    if (state->mode == FADE_MODE_IN) {
        palette_step = progress;
    } else {
        palette_step = (u8)(FADE_STEPS - progress);
    }

    fade_apply(palette_step);
    state->step = palette_step;
    state->frame++;
    if (state->frame >= state->duration_frames) {
        state->active = 0;
    }
    return state->active;
}

void fade_cancel(fade_state_t *state) {
    state->active = 0;
}

static void fade_loop(u8 frames, u8 mode) {
    fade_state_t s;

    if (!fade_begin(&s, frames, mode)) return;
    while (s.active) {
        video_vsync();
        fade_step(&s);
    }
}

static void fade_loop_music(u8 frames, u8 mode, u8 pt3_block) {
    fade_state_t s;

    if (!fade_begin(&s, frames, mode)) return;
    while (s.active) {
        video_vsync();
        ay_pt3_play(pt3_block);
        fade_step(&s);
    }
}

void fade_out(u8 frames) {
    fade_loop(frames, FADE_MODE_OUT);
}

void fade_in(u8 frames) {
    fade_loop(frames, FADE_MODE_IN);
}

void fade_out_music(u8 frames, u8 pt3_block) {
    fade_loop_music(frames, FADE_MODE_OUT, pt3_block);
}

void fade_in_music(u8 frames, u8 pt3_block) {
    fade_loop_music(frames, FADE_MODE_IN, pt3_block);
}
