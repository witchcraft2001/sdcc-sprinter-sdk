#ifndef _MULTI_IMG_H
#define _MULTI_IMG_H

#include <sprinter/gfx.h>

#define MULTI_IMG_COUNT       5
#define MULTI_IMG_PAGES       5
#define MULTI_IMG_PALETTE_COUNT 12

static const video_rgb6_t multi_palette[] = {
    {0, 0, 0},
    {63, 63, 63},
    {8, 10, 30},
    {10, 22, 12},
    {15, 15, 15},
    {50, 40, 7},
    {35, 7, 7},
    {55, 55, 25},
    {63, 50, 50},
    {45, 50, 63},
    {45, 60, 45},
    {50, 50, 50},
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
