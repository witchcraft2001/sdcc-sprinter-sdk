/*
 * 28_multi_image — load several full-screen images into separate DSS blocks
 * and switch between them on screen without re-reading from disk.
 *
 * Each .GFX file is a raw 320x256 8-bit blob occupying MULTI_IMG_PAGES EMM
 * pages. After loading all five, gfx_select_resource_block() is used to point
 * the gfx page cache at whichever block we want to draw next.
 */

#include <sprinter.h>
#include <sprinter/gfx.h>
#include "res.h"

static u8 blocks[MULTI_IMG_COUNT];

static void free_blocks(u8 count) {
    u8 i;

    for (i = 0; i < count; i++) {
        if (blocks[i] != 0xFF)
            dss_freemem(blocks[i]);
    }
}

static u8 load_all(void) {
    u8 i;

    for (i = 0; i < MULTI_IMG_COUNT; i++)
        blocks[i] = 0xFF;

    for (i = 0; i < MULTI_IMG_COUNT; i++) {
        dss_puts("  loading ");
        dss_puts(multi_paths[i]);
        dss_puts("\r\n");

        blocks[i] = dss_getmem_pages(MULTI_IMG_PAGES);
        if (blocks[i] == 0xFF) return 0;
        if (gfx_load_resource_pages(multi_paths[i], blocks[i],
                                    MULTI_IMG_PAGES) <= 0)
            return 0;
    }
    return 1;
}

static void show(u8 index) {
    u8 hidden;

    hidden = gfx_get_active_screen() ^ 1;
    gfx_select_resource_block(blocks[index], MULTI_IMG_PAGES);
    gfx_draw_image_page(hidden, 0, 0, blocks[index],
                        &multi_images[index], GFX_OPAQUE);
    gfx_flip();
}

static void run_demo(void) {
    static const u8 order[] = { 0, 1, 2, 3, 4, 2, 4, 0, 3, 1 };
    u8 i;

    video_setmode(VMODE_320);
    video_setpal_range(0, MULTI_IMG_PALETTE_COUNT, multi_palette);

    for (i = 0; i < sizeof(order); i++) {
        show(order[i]);
        if (dss_waitkey() == 27) break;   /* ESC */
    }

    video_setmode(VMODE_TEXT80);
}

void main(void) {
    dss_puts("Multi-image demo\r\n");
    dss_puts("Loading all images into separate DSS blocks...\r\n");

    if (load_all()) {
        dss_puts("All loaded. Press any key to advance, ESC to quit.\r\n");
        dss_waitkey();
        run_demo();
    } else {
        dss_puts("Load failed.\r\n");
        dss_waitkey();
    }

    free_blocks(MULTI_IMG_COUNT);
}
