#ifndef _DEMO_GFX_H
#define _DEMO_GFX_H

#include <sprinter/gfx.h>

#define DEMO_GFX_RESOURCE_COUNT 2
#define DEMO_GFX_PALETTE_COUNT 8

static const video_rgb8_t demo_gfx_palette[] = {
    {255, 240, 64},
    {64, 224, 96},
    {64, 192, 255},
    {144, 96, 255},
    {255, 96, 192},
    {255, 255, 255},
    {255, 32, 32},
    {255, 160, 32},
};

static const gfx_resource_t demo_gfx_resources[] = {
    {0, 0x0030, 256, 16, 16, 0},
    {0, 0x0130, 576, 24, 24, 0},
};

#endif /* _DEMO_GFX_H */
