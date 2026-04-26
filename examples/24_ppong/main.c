#include <sprinter.h>
#include <sprinter/gfx.h>
#include "res.h"

#define RES_BG0         0
#define RES_BG1         1
#define RES_BALL        2
#define RES_PAD_TOP     3
#define RES_PAD_MID     4
#define RES_PAD_BOT     5
#define RES_FONT_FIRST  6

#define FIELD_TOP       32
#define BALL_SIZE       16
#define PADDLE_W        16
#define PADDLE_H        48
#define CPU_X           16
#define PLAYER_X        288
#define SCORE_Y         8
#define KEY_SCAN_UP     0x58
#define KEY_SCAN_DOWN   0x52

static const char font_chars[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789:-. ";

typedef struct game_state {
    i16 ball_x;
    i16 ball_y;
    i8  ball_dx;
    i8  ball_dy;
    u8  player_y;
    u8  cpu_y;
    u8  player_score;
    u8  cpu_score;
    i8  player_v;
    u8  player_hold;
} game_state_t;

typedef struct draw_state {
    u16 ball_x[2];
    u8  ball_y[2];
    u8  player_y[2];
    u8  cpu_y[2];
    u8  mask;
} draw_state_t;

static void show_screen0(void) {
    if (gfx_get_active_screen() != GFX_SCREEN_0)
        gfx_flip();
}

static u8 font_id(char ch) {
    u8 i;

    for (i = 0; font_chars[i]; i++) {
        if (font_chars[i] == ch)
            return RES_FONT_FIRST + i;
    }
    return RES_FONT_FIRST + sizeof(font_chars) - 2;
}

static void draw_text(u8 screen, u16 x, u8 y, u8 page, const char *s) {
    while (*s) {
        gfx_draw_image_resource(screen, x, y, page, ppong_images, font_id(*s),
                                GFX_MASKED | GFX_VRAM_ONLY);
        x += 16;
        s++;
    }
}

static void draw_digit(u8 screen, u16 x, u8 y, u8 page, u8 value) {
    gfx_draw_image_resource(screen, x, y, page, ppong_images,
                            font_id((char)('0' + (value % 10))),
                            GFX_MASKED | GFX_VRAM_ONLY);
}

static void restore_score(u8 screen) {
    gfx_restore_rect(screen, 0, 0, 160, FIELD_TOP);
    gfx_restore_rect(screen, 160, 0, 160, FIELD_TOP);
}

static void draw_score(u8 screen, u8 page, const game_state_t *g) {
    draw_text(screen, 16, SCORE_Y, page, "CPU");
    draw_digit(screen, 80, SCORE_Y, page, g->cpu_score);
    draw_text(screen, 128, SCORE_Y, page, "-");
    draw_digit(screen, 160, SCORE_Y, page, g->player_score);
    draw_text(screen, 208, SCORE_Y, page, "YOU");
}

static void draw_paddle(u8 screen, u16 x, u8 y, u8 page) {
    gfx_draw_image_resource(screen, x, y, page, ppong_images, RES_PAD_TOP,
                            GFX_MASKED | GFX_VRAM_ONLY);
    gfx_draw_image_resource(screen, x, y + 16, page, ppong_images, RES_PAD_MID,
                            GFX_MASKED | GFX_VRAM_ONLY);
    gfx_draw_image_resource(screen, x, y + 32, page, ppong_images, RES_PAD_BOT,
                            GFX_MASKED | GFX_VRAM_ONLY);
}

static void restore_paddle(u8 screen, u16 x, u8 y) {
    gfx_restore_sprite16(screen, x, y);
    gfx_restore_sprite16(screen, x, y + 16);
    gfx_restore_sprite16(screen, x, y + 32);
}

static void reset_ball(game_state_t *g, i8 dir) {
    g->ball_x = 152;
    g->ball_y = 120;
    g->ball_dx = dir;
    g->ball_dy = (g->player_score + g->cpu_score) & 1 ? 2 : -2;
}

static void reset_game(game_state_t *g) {
    g->player_y = 104;
    g->cpu_y = 104;
    g->player_score = 0;
    g->cpu_score = 0;
    g->player_v = 0;
    g->player_hold = 0;
    reset_ball(g, 3);
}

static void process_input(game_state_t *g, u8 *quit) {
    dss_key_t key;

    while (dss_scankey(&key)) {
        if (key.ascii == 27) {
            *quit = 1;
            return;
        }
        if (key.scan == KEY_SCAN_UP || key.ascii == 'w' || key.ascii == 'W') {
            g->player_v = -4;
            g->player_hold = 7;
        } else if (key.scan == KEY_SCAN_DOWN || key.ascii == 's' || key.ascii == 'S') {
            g->player_v = 4;
            g->player_hold = 7;
        }
    }
}

static u8 clamp_paddle(i16 y) {
    if (y < FIELD_TOP) return FIELD_TOP;
    if (y > (i16)(SCREEN_H_320 - PADDLE_H)) return SCREEN_H_320 - PADDLE_H;
    return (u8)y;
}

static u8 overlaps(i16 ax, i16 ay, i16 aw, i16 ah, i16 bx, i16 by, i16 bw, i16 bh) {
    return ax < bx + bw && ax + aw > bx && ay < by + bh && ay + ah > by;
}

static void update_game(game_state_t *g) {
    i16 target;

    if (g->player_hold) {
        g->player_y = clamp_paddle((i16)g->player_y + g->player_v);
        g->player_hold--;
    } else {
        g->player_v = 0;
    }

    target = g->ball_y + BALL_SIZE / 2 - PADDLE_H / 2;
    if (target > (i16)g->cpu_y + 3)
        g->cpu_y = clamp_paddle((i16)g->cpu_y + 3);
    else if (target < (i16)g->cpu_y - 3)
        g->cpu_y = clamp_paddle((i16)g->cpu_y - 3);

    g->ball_x += g->ball_dx;
    g->ball_y += g->ball_dy;

    if (g->ball_y < FIELD_TOP) {
        g->ball_y = FIELD_TOP;
        g->ball_dy = -g->ball_dy;
    } else if (g->ball_y > (i16)(SCREEN_H_320 - BALL_SIZE)) {
        g->ball_y = SCREEN_H_320 - BALL_SIZE;
        g->ball_dy = -g->ball_dy;
    }

    if (g->ball_dx < 0 &&
        overlaps(g->ball_x, g->ball_y, BALL_SIZE, BALL_SIZE,
                 CPU_X, g->cpu_y, PADDLE_W, PADDLE_H)) {
        g->ball_x = CPU_X + PADDLE_W;
        g->ball_dx = 3;
        g->ball_dy += (i8)(((g->ball_y + 8) - (g->cpu_y + 24)) / 10);
    }

    if (g->ball_dx > 0 &&
        overlaps(g->ball_x, g->ball_y, BALL_SIZE, BALL_SIZE,
                 PLAYER_X, g->player_y, PADDLE_W, PADDLE_H)) {
        g->ball_x = PLAYER_X - BALL_SIZE;
        g->ball_dx = -3;
        g->ball_dy += (i8)(((g->ball_y + 8) - (g->player_y + 24)) / 10);
    }

    if (g->ball_dy > 4) g->ball_dy = 4;
    if (g->ball_dy < -4) g->ball_dy = -4;
    if (g->ball_dy == 0) g->ball_dy = 1;

    if (g->ball_x < 0) {
        if (g->player_score < 9) g->player_score++;
        reset_ball(g, 3);
    } else if (g->ball_x > (i16)(SCREEN_W_320 - BALL_SIZE)) {
        if (g->cpu_score < 9) g->cpu_score++;
        reset_ball(g, -3);
    }

    if (g->player_score == 9 || g->cpu_score == 9) {
        g->player_score = 0;
        g->cpu_score = 0;
    }
}

static void draw_objects(u8 screen, u8 page, const game_state_t *g) {
    draw_paddle(screen, PLAYER_X, g->player_y, page);
    draw_paddle(screen, CPU_X, g->cpu_y, page);
    gfx_draw_image_resource(screen, (u16)g->ball_x, (u8)g->ball_y, page,
                            ppong_images, RES_BALL, GFX_MASKED | GFX_VRAM_ONLY);
}

static void restore_objects(u8 screen, const draw_state_t *d) {
    if (!(d->mask & (1 << screen))) return;
    restore_paddle(screen, PLAYER_X, d->player_y[screen]);
    restore_paddle(screen, CPU_X, d->cpu_y[screen]);
    gfx_restore_sprite16(screen, d->ball_x[screen], d->ball_y[screen]);
}

static void remember_objects(u8 screen, draw_state_t *d, const game_state_t *g) {
    d->ball_x[screen] = (u16)g->ball_x;
    d->ball_y[screen] = (u8)g->ball_y;
    d->player_y[screen] = g->player_y;
    d->cpu_y[screen] = g->cpu_y;
    d->mask |= (1 << screen);
}

static void draw_backgrounds(u8 page) {
    gfx_draw_image_resource(GFX_SCREEN_0, 0, 0, page, ppong_images, RES_BG0, GFX_OPAQUE);
    gfx_draw_image_resource(GFX_SCREEN_0, 0, 128, page, ppong_images, RES_BG1, GFX_OPAQUE);
    gfx_draw_image_resource(GFX_SCREEN_1, 0, 0, page, ppong_images, RES_BG0, GFX_OPAQUE);
    gfx_draw_image_resource(GFX_SCREEN_1, 0, 128, page, ppong_images, RES_BG1, GFX_OPAQUE);
}

static void run_game(u8 page) {
    game_state_t g;
    draw_state_t d;
    u8 hidden;
    u8 quit;

    reset_game(&g);
    d.mask = 0;
    quit = 0;

    draw_backgrounds(page);
    draw_score(GFX_SCREEN_0, page, &g);
    draw_score(GFX_SCREEN_1, page, &g);
    draw_objects(GFX_SCREEN_0, page, &g);
    draw_objects(GFX_SCREEN_1, page, &g);
    remember_objects(GFX_SCREEN_0, &d, &g);
    remember_objects(GFX_SCREEN_1, &d, &g);

    while (!quit) {
        process_input(&g, &quit);
        update_game(&g);

        hidden = gfx_get_active_screen() ^ 1;
        restore_objects(hidden, &d);
        restore_score(hidden);
        draw_score(hidden, page, &g);
        draw_objects(hidden, page, &g);
        remember_objects(hidden, &d, &g);

        video_vsync();
        gfx_flip();
    }
}

void main(void) {
    u8 block;

    dss_puts("PPONG demo\r\n");
    dss_puts("Loading PPONG.GFX...\r\n");
    block = dss_getmem_pages(PPONG_PAGE_COUNT);
    if (block != 0xFF && gfx_load_resource_pages("PPONG.GFX", block, PPONG_PAGE_COUNT) > 0) {
        dss_puts("Cursor up/down, ESC exits.\r\n");
        video_setmode(VMODE_320);
        show_screen0();
        video_setpal_range(0, PPONG_PALETTE_COUNT, ppong_palette);
        run_game(block);
    } else {
        dss_puts("Resource load failed.\r\n");
        dss_waitkey();
    }

    if (block != 0xFF)
        dss_freemem(block);
    video_setmode(VMODE_TEXT80);
}
