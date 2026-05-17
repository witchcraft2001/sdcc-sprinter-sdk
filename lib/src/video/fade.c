#include <sprinter/fade.h>
#include <sprinter/video.h>
#include <sprinter/ay.h>

extern void video_pal_accel_push(const u8 *src_channels);

/*
 * Step-paced palette fade engine.
 *
 * The hot path uses the Sprinter accelerator burst (see
 * lib/src/video/fade_pal.s) which transfers 256 bytes per accel
 * trigger. To make use of it, source and intermediate buffers are
 * laid out as three contiguous 256-byte channels: B[256], G[256],
 * R[256] (NOT interleaved RGB triples).
 *
 * fade_capture_palette() transposes the caller's video_rgb6_t array
 * into this channels layout once. fade_step() builds the faded
 * channels via a 64-byte component LUT (recomputed only when the
 * step value changes) and pushes the result with six accel bursts
 * - three per palette page x two hardware palette pages.
 *
 * fade_step() does NOT wait for vsync and does NOT play music; the
 * caller drives the frame loop. fade_in/out + fade_in/out_music are
 * blocking wrappers.
 */

static u8 fade_source_channels[768];    /* B[256] G[256] R[256], 0..63 raw */
static u8 fade_temp_channels[768];      /* shifted << 2, ready for accel  */
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
    u16 i;
    u8  v;

    fade_build_lut(step);

    /* Single linear pass over all 768 source bytes - channel layout
     * is irrelevant to the LUT lookup itself. */
    for (i = 0; i < 768; i++) {
        v = fade_lut[fade_source_channels[i]];
        fade_temp_channels[i] = (u8)(v << 2);
    }

    video_pal_accel_push(fade_temp_channels);
}

void fade_capture_palette(const video_rgb6_t *palette) {
    u16 i;
    u8 *b_chan;
    u8 *g_chan;
    u8 *r_chan;

    b_chan = fade_source_channels + 0;
    g_chan = fade_source_channels + 256;
    r_chan = fade_source_channels + 512;
    for (i = 0; i < 256; i++) {
        b_chan[i] = palette[i].b;
        g_chan[i] = palette[i].g;
        r_chan[i] = palette[i].r;
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
