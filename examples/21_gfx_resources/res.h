#ifndef _DEMO_GFX_H
#define _DEMO_GFX_H

#include <sprinter/gfx.h>

#define DEMO_GFX_RESOURCE_COUNT 2
#define DEMO_GFX_PALETTE_COUNT 8

static const video_rgb6_t demo_gfx_palette[] = {
    {63, 60, 16},
    {16, 56, 24},
    {16, 48, 63},
    {36, 24, 63},
    {63, 24, 48},
    {63, 63, 63},
    {63, 8, 8},
    {63, 40, 8},
};

static const gfx_resource_t demo_gfx_resources[] = {
    {0, 0x0030, 256, 16, 16, 0},
    {0, 0x0130, 576, 24, 24, 0},
};

#endif /* _DEMO_GFX_H */
