# Palette Fade Guide

The SDK ships a small palette fade module (`<sprinter/fade.h>`) for fading the active 256-colour palette in and out without dropping music or input handling. The module is split into two layers:

- a **step-driven core** that applies exactly one palette step per call and returns control immediately, and
- a few **blocking convenience wrappers** for short demos and old-style code.

The step API is the main one: it does not wait for vsync and it does not play music, so the calling frame loop stays in charge of pacing, sound, animation and input.

## Why a step API?

`video_setpal_range()` calls `bios_setpal()` once per colour, which is too slow to drive a 256-colour fade at 50 Hz while PT3 music is playing. The fade module:

1. captures the source palette once into RAM,
2. precomputes a 64-entry brightness lookup table for the current step (one multiply per component value, recomputed only when the step changes),
3. expands the LUT into a hardware-format buffer on the stack, and
4. pushes the result to both hardware palette pages with `video_setpal256_fast()` (four BIOS range calls, no per-colour overhead).

The whole step finishes well inside a 50 Hz frame, so PT3 playback and your own per-frame work continue uninterrupted.

## API summary

```c
#include <sprinter/fade.h>

#define FADE_MODE_OUT   0
#define FADE_MODE_IN    1
#define FADE_STEPS      32

typedef struct fade_state {
    u8  active;
    u8  duration_frames;
    u8  mode;
    u8  frame;
    u8  step;
    u16 accumulator;
} fade_state_t;

void fade_capture_palette(const video_rgb6_t *palette);

u8   fade_begin(fade_state_t *state, u8 duration_frames, u8 mode);
u8   fade_step(fade_state_t *state);
void fade_cancel(fade_state_t *state);

void fade_out(u8 frames);
void fade_in(u8 frames);
void fade_out_music(u8 frames, u8 pt3_block);
void fade_in_music(u8 frames, u8 pt3_block);
```

### `fade_capture_palette(palette)`

Copies the 256 RGB6 source colours into an internal buffer. Call this once after installing the final palette in hardware (e.g. via `video_setpal256_fast()` or `video_setpal_range()`). Every subsequent `fade_in()`/`fade_out()` treats this captured palette as the "full brightness" reference.

### `fade_begin(state, duration_frames, mode)`

Initialises `state` for a step-driven fade.

- `duration_frames` - total number of `fade_step()` calls until the fade is done. `0` immediately applies the final state (black for `FADE_MODE_OUT`, captured palette for `FADE_MODE_IN`) and leaves `state->active` cleared.
- `mode` - `FADE_MODE_OUT` or `FADE_MODE_IN`.

Returns `state->active` (1 if more steps remain, 0 if the fade completed immediately).

### `fade_step(state)`

Applies exactly one palette step and returns. Does **not** wait for vsync and does **not** play music. The caller decides when to call `video_vsync()`, when to call `ay_pt3_play()`, and what other per-frame work to do. Returns 1 while the fade is still active, 0 when the last step has been applied.

### `fade_cancel(state)`

Aborts the fade without touching the current palette. Useful if the player presses a "skip" key mid-fade.

### Blocking wrappers

The four wrappers (`fade_out`, `fade_in`, `fade_out_music`, `fade_in_music`) are convenience helpers built directly on top of `fade_begin`/`fade_step`. They wait for vsync between steps and optionally call `ay_pt3_play(pt3_block)` so PT3 music keeps playing. Use them for splash screens and short cutscenes; for actual gameplay, prefer the step API so the main loop stays in control.

## Recommended usage with PT3

```c
fade_state_t fade;

video_setpal256_fast(my_palette);
fade_capture_palette(my_palette);
ay_pt3_init(music_block);

fade_begin(&fade, 16, FADE_MODE_OUT);
while (fade.active) {
    video_vsync();
    ay_pt3_play(music_block);
    /* run game-state update, input scan, sprite refresh, ... */
    fade_step(&fade);
}
```

Each iteration does one `vsync` + one music tick + one fade step + your own work, in the order you choose. Because `fade_step()` returns control quickly, PT3 timing stays solid and gameplay logic still gets to run during the fade.

## Memory footprint

The module keeps a single captured palette (768 bytes) plus a 64-byte component LUT in static memory. The hardware-format buffer used to feed BIOS is built on the stack inside the asm helper, so it does not consume static RAM. Plan for roughly 1 KB of free stack while a fade step is in flight.

## See also

- `<sprinter/video.h>` - `video_setpal256_fast()` (`docs/en/06_graphics_guide.md`, "Method 4")
- `examples/29_fade/` - end-to-end demo with PT3 music, animated border and step-driven fade
