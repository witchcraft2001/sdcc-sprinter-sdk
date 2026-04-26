#include <sprinter.h>
#include <sprinter/gfx.h>
#include "res.h"

#define BALL_COUNT 4
#define BALL_SIZE  16
#define BG_WIDTH   320
#define BG_HEIGHT  200

typedef struct ball {
    u16 x;
    u16 old_x;
    u8  y;
    u8  old_y;
    i8  dx;
    i8  dy;
    u8  image_id;
} ball_t;

static void show_screen0(void) {
    if (gfx_get_active_screen() != GFX_SCREEN_0)
        gfx_flip();
}

static u8 rnd8(void) {
    static u8 state = 0xA7;

    state = (state << 1) ^ ((state & 0x80) ? 0x1D : 0);
    return state;
}

static void clamp_speed(i8 *v) {
    if (*v > 3) *v = 3;
    if (*v < -3) *v = -3;
    if (*v == 0) *v = 1;
}

static void update_ball(ball_t *b) {
    i16 nx;
    i16 ny;

    b->old_x = b->x;
    b->old_y = b->y;

    if ((rnd8() & 0x3F) == 0) {
        b->dx += (rnd8() & 1) ? 1 : -1;
        clamp_speed(&b->dx);
    }
    if ((rnd8() & 0x3F) == 0) {
        b->dy += (rnd8() & 1) ? 1 : -1;
        clamp_speed(&b->dy);
    }

    nx = (i16)b->x + b->dx;
    ny = (i16)b->y + b->dy;

    if (nx < 0) {
        nx = 0;
        b->dx = -b->dx;
    } else if (nx > (i16)(BG_WIDTH - BALL_SIZE)) {
        nx = BG_WIDTH - BALL_SIZE;
        b->dx = -b->dx;
    }

    if (ny < 0) {
        ny = 0;
        b->dy = -b->dy;
    } else if (ny > (i16)(BG_HEIGHT - BALL_SIZE)) {
        ny = BG_HEIGHT - BALL_SIZE;
        b->dy = -b->dy;
    }

    b->x = (u16)nx;
    b->y = (u8)ny;
}

static void run_demo(u8 base_page) {
    u8 i;
    ball_t balls[BALL_COUNT] = {
        { 28,  28,  24,  24,  2,  1, 1 },
        {112, 112,  56,  56, -2,  2, 2 },
        {196, 196,  94,  94,  3, -1, 3 },
        {276, 276, 150, 150, -3, -2, 4 }
    };

    gfx_draw_image_resource(GFX_SCREEN_0, 0, 0, base_page,
                            balls_demo_images, 0, GFX_OPAQUE);

    for (i = 0; i < BALL_COUNT; i++) {
        gfx_draw_image_resource(GFX_SCREEN_0, balls[i].x, balls[i].y, base_page,
                                balls_demo_images, balls[i].image_id,
                                GFX_MASKED | GFX_VRAM_ONLY);
    }

    while (!dss_kbhit()) {
        video_vsync();

        for (i = 0; i < BALL_COUNT; i++)
            gfx_restore_sprite16(GFX_SCREEN_0, balls[i].old_x, balls[i].old_y);

        for (i = 0; i < BALL_COUNT; i++)
            update_ball(&balls[i]);

        for (i = 0; i < BALL_COUNT; i++) {
            gfx_draw_image_resource(GFX_SCREEN_0, balls[i].x, balls[i].y, base_page,
                                    balls_demo_images, balls[i].image_id,
                                    GFX_MASKED | GFX_VRAM_ONLY);
        }
    }
    dss_waitkey();
}

void main(void) {
    u8 block;

    dss_puts("Balls demo\r\n");
    dss_puts("Loading BALLS.GFX...\r\n");
    block = dss_getmem_pages(BALLS_DEMO_PAGE_COUNT);
    if (block != 0xFF &&
        gfx_load_resource_pages("BALLS.GFX", block, BALLS_DEMO_PAGE_COUNT) > 0) {
        dss_puts("Drawing background...\r\n");
        video_setmode(VMODE_320);
        show_screen0();
        video_setpal_range(0, BALLS_DEMO_PALETTE_COUNT, balls_demo_palette);
        run_demo(block);
    } else {
        dss_puts("Resource load failed.\r\n");
        dss_waitkey();
    }

    if (block != 0xFF)
        dss_freemem(block);
    video_setmode(VMODE_TEXT80);
}
