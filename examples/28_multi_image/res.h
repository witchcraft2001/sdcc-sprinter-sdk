#ifndef _MULTI_IMG_H
#define _MULTI_IMG_H

#include <sprinter/gfx.h>

#define MULTI_IMG_COUNT       5
#define MULTI_IMG_PAGES       5
#define MULTI_IMG_PALETTE_COUNT 12

static const video_rgb8_t multi_palette[] = {
    {0, 0, 0},
    {255, 255, 255},
    {32, 40, 120},
    {40, 90, 50},
    {60, 60, 60},
    {200, 160, 30},
    {140, 30, 30},
    {220, 220, 100},
    {255, 200, 200},
    {180, 200, 255},
    {180, 240, 180},
    {200, 200, 200},
};

static const char *const multi_paths[MULTI_IMG_COUNT] = {
    "SPLASH.GFX",
    "MENU.GFX",
    "LOADING.GFX",
    "WON.GFX",
    "OVER.GFX",
};

static const char *const multi_names[MULTI_IMG_COUNT] = {
    "splash",
    "menu",
    "loading",
    "won",
    "over",
};

static const gfx_image_t multi_images[MULTI_IMG_COUNT] = {
    {0, 0x0000, 0x4001, 320, 0, GFX_RESF_COLUMNS},
    {0, 0x0000, 0x4001, 320, 0, GFX_RESF_COLUMNS},
    {0, 0x0000, 0x4001, 320, 0, GFX_RESF_COLUMNS},
    {0, 0x0000, 0x4001, 320, 0, GFX_RESF_COLUMNS},
    {0, 0x0000, 0x4001, 320, 0, GFX_RESF_COLUMNS},
};

#endif /* _MULTI_IMG_H */
