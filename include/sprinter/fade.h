/**
 * fade.h - Step-paced palette fade helper.
 *
 * Provides a small fade engine that applies one palette-step per call.
 * The step API does not wait for vsync and does not play music: the
 * caller drives the frame loop so fades can be combined with PT3
 * playback, sprite updates and input handling.
 *
 * Typical usage:
 *
 *     fade_capture_palette(my_palette);
 *     fade_state_t fade;
 *     fade_begin(&fade, 16, FADE_MODE_OUT);
 *     while (fade.active) {
 *         video_vsync();
 *         ay_pt3_play(music_block);
 *         fade_step(&fade);
 *     }
 *
 * The blocking convenience wrappers (fade_in(), fade_out(),
 * fade_in_music(), fade_out_music()) are thin helpers over the step
 * API for short demos.
 */

#ifndef _SPRINTER_FADE_H
#define _SPRINTER_FADE_H

#include <sprinter/types.h>
#include <sprinter/video.h>

#define FADE_MODE_OUT       0   /* Fade from captured palette to black */
#define FADE_MODE_IN        1   /* Fade from black to captured palette */
#define FADE_STEPS          32  /* Number of brightness levels (1..32) */

/** Runtime state for a single fade. Reset by fade_begin(). */
typedef struct fade_state {
    u8  active;             /* 1 while the fade is still in progress */
    u8  duration_frames;    /* total number of steps requested */
    u8  mode;               /* FADE_MODE_OUT / FADE_MODE_IN */
    u8  frame;              /* steps applied so far */
    u8  step;               /* last brightness step applied (0..FADE_STEPS) */
    u16 accumulator;        /* internal: progress in 1/duration_frames units */
} fade_state_t;

/** Capture the source palette for subsequent fades.
 *
 *  Copies 256 RGB8 entries (0..255 per channel) from `palette` into an
 *  internal buffer. Call once after you have loaded the desired final
 *  palette into hardware (e.g. via video_setpal_range() or
 *  video_setpal256_fast()). Subsequent fade_in/out operations use this
 *  captured palette as the "full brightness" reference. */
void fade_capture_palette(const video_rgb8_t *palette);

/** Initialise a fade state for a step-driven fade.
 *
 *  duration_frames - total number of fade_step() calls until done.
 *                    A value of 0 immediately applies the final state
 *                    (black for FADE_MODE_OUT, captured palette for
 *                    FADE_MODE_IN) and marks the fade inactive.
 *  mode            - FADE_MODE_OUT or FADE_MODE_IN.
 *
 *  Returns the value of state->active after init (1 if more steps
 *  remain, 0 if the fade was applied immediately). */
u8 fade_begin(fade_state_t *state, u8 duration_frames, u8 mode);

/** Apply one fade step and advance the state.
 *
 *  Does not wait for vsync and does not play music. The caller is
 *  responsible for frame pacing and any other per-frame work. Returns
 *  1 while the fade is still active, 0 when the last step has been
 *  applied. */
u8 fade_step(fade_state_t *state);

/** Abort a fade without changing the current palette. */
void fade_cancel(fade_state_t *state);

/** Blocking fade-out: drives video_vsync()+fade_step() until done.
 *  Equivalent to fade_begin(FADE_MODE_OUT) plus a vsync loop.
 *  Requires a prior fade_capture_palette() call. */
void fade_out(u8 frames);

/** Blocking fade-in: drives video_vsync()+fade_step() until done. */
void fade_in(u8 frames);

/** Same as fade_out() but also calls ay_pt3_play(pt3_block) after each
 *  video_vsync() so PT3 music keeps playing while the fade runs. */
void fade_out_music(u8 frames, u8 pt3_block);

/** Same as fade_in() but keeps PT3 playback running. */
void fade_in_music(u8 frames, u8 pt3_block);

#endif /* _SPRINTER_FADE_H */
