#include <sprinter.h>
#include <sprinter/gfx.h>
#include <string.h>
#include "res.h"

static void clear_screens(void) {
    volatile u8 *vram = (volatile u8 *)0xC000;
    u8 y;

    video_mapvram(3, VRAM_PAGE_BASE);
    y = 0;
    do {
        outp(PORT_PAL_ADDR, y);
        memset((void *)vram, 0, 320);
        memset((void *)(vram + 320), 0, 320);
        y++;
    } while (y != 0);
    video_safe_porty();
}

static void show_screen0(void) {
    if (gfx_get_active_screen() != GFX_SCREEN_0)
        gfx_flip();
}

void main(void) {
    u8 page;

    video_setmode(VMODE_320);
    show_screen0();
    video_setpal_range(0, DEMO_GFX_PALETTE_COUNT, demo_gfx_palette);
    clear_screens();

    page = dss_getmem();
    if (page != 0xFF && gfx_load_resource_pages("GFXDEMO.GFX", page, 1) > 0) {
        gfx_draw_resource(GFX_SCREEN_0, 72, 70, page, demo_gfx_resources, 0, GFX_MASKED);
        gfx_draw_resource(GFX_SCREEN_0, 140, 100, page, demo_gfx_resources, 1, GFX_MASKED);
        gfx_copy_screen(GFX_SCREEN_1, GFX_SCREEN_0);
        gfx_flip();
        dss_waitkey();
        gfx_flip();
        dss_waitkey();
        dss_freemem(page);
    } else {
        dss_waitkey();
    }

    video_setmode(VMODE_TEXT80);
}
