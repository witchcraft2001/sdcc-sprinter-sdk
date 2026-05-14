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
#define MUSIC_PAGE_COUNT 1
#define BORDER_BLACK    0
#define BORDER_BLUE     1
#define BORDER_PLAYER   2
#define BORDER_AFTER    6

#define CONTROL_JOYSTICK 0
#define CONTROL_FE       1
#define CONTROL_DSS      2

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

static void debug_border(u8 color) {
    outp(PORT_KEYB, color & 7);
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
                            GFX_OPAQUE | GFX_VRAM_ONLY);
    gfx_draw_image_resource(screen, x, y + 16, page, ppong_images, RES_PAD_MID,
                            GFX_OPAQUE | GFX_VRAM_ONLY);
    gfx_draw_image_resource(screen, x, y + 32, page, ppong_images, RES_PAD_BOT,
                            GFX_OPAQUE | GFX_VRAM_ONLY);
}

static void restore_paddle(u8 screen, u16 x, u8 y) {
    gfx_restore_rect(screen, x, y, PADDLE_W, PADDLE_H);
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

static void read_quit_events(u8 *quit) {
    dss_key_t key;

    while (dss_scankey(&key)) {
        if (key.ascii == 27) {
            *quit = 1;
            return;
        }
    }
}

static void process_input_joystick(game_state_t *g, u8 *quit) {
    u8 joy;
    bool up;
    bool down;

    read_quit_events(quit);
    if (*quit)
        return;

    joy = joystick();
    up = (joy & JOY_UP) != 0;
    down = (joy & JOY_DOWN) != 0;
    g->player_hold = 0;

    if (up && !down)
        g->player_v = -4;
    else if (down && !up)
        g->player_v = 4;
    else
        g->player_v = 0;
}

static void process_input_fe(game_state_t *g, u8 *quit) {
    bool up;
    bool down;

    read_quit_events(quit);
    if (*quit)
        return;

    up = keyb_pressed(KEYB_W);
    down = keyb_pressed(KEYB_S);
    g->player_hold = 0;

    if (up && !down)
        g->player_v = -4;
    else if (down && !up)
        g->player_v = 4;
    else
        g->player_v = 0;
}

static void process_input_dss(game_state_t *g, u8 *quit) {
    dss_key_t key;

    if (dss_scankey(&key)) {
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

static void process_input(game_state_t *g, u8 *quit, u8 control) {
    if (control == CONTROL_JOYSTICK)
        process_input_joystick(g, quit);
    else if (control == CONTROL_DSS)
        process_input_dss(g, quit);
    else
        process_input_fe(g, quit);
}

static u8 clamp_paddle(i16 y) {
    if (y < FIELD_TOP) return FIELD_TOP;
    if (y > (i16)(SCREEN_H_320 - PADDLE_H)) return SCREEN_H_320 - PADDLE_H;
    return (u8)y;
}

static u8 overlaps(i16 ax, i16 ay, i16 aw, i16 ah, i16 bx, i16 by, i16 bw, i16 bh) {
    return ax < bx + bw && ax + aw > bx && ay < by + bh && ay + ah > by;
}

static u8 ball_overlaps_paddle(u16 ball_x, u8 ball_y, u16 paddle_x, u8 paddle_y) {
    return overlaps((i16)ball_x, (i16)ball_y, BALL_SIZE, BALL_SIZE,
                    (i16)paddle_x, (i16)paddle_y, PADDLE_W, PADDLE_H);
}

static void update_game(game_state_t *g) {
    i16 target;

    if (g->player_v)
        g->player_y = clamp_paddle((i16)g->player_y + g->player_v);

    if (g->player_hold) {
        g->player_hold--;
        if (!g->player_hold)
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

static void remember_objects(u8 screen, draw_state_t *d, const game_state_t *g) {
    d->ball_x[screen] = (u16)g->ball_x;
    d->ball_y[screen] = (u8)g->ball_y;
    d->player_y[screen] = g->player_y;
    d->cpu_y[screen] = g->cpu_y;
    d->mask |= (1 << screen);
}

static void render_objects(u8 screen, u8 page, draw_state_t *d, const game_state_t *g) {
    u8 player_dirty;
    u8 cpu_dirty;

    player_dirty = 1;
    cpu_dirty = 1;
    if (d->mask & (1 << screen)) {
        player_dirty =
            d->player_y[screen] != g->player_y ||
            ball_overlaps_paddle(d->ball_x[screen], d->ball_y[screen],
                                 PLAYER_X, d->player_y[screen]) ||
            ball_overlaps_paddle((u16)g->ball_x, (u8)g->ball_y,
                                 PLAYER_X, g->player_y);
        cpu_dirty =
            d->cpu_y[screen] != g->cpu_y ||
            ball_overlaps_paddle(d->ball_x[screen], d->ball_y[screen],
                                 CPU_X, d->cpu_y[screen]) ||
            ball_overlaps_paddle((u16)g->ball_x, (u8)g->ball_y,
                                 CPU_X, g->cpu_y);

        if (player_dirty)
            restore_paddle(screen, PLAYER_X, d->player_y[screen]);
        if (cpu_dirty)
            restore_paddle(screen, CPU_X, d->cpu_y[screen]);
        gfx_restore_sprite16(screen, d->ball_x[screen], d->ball_y[screen]);
    }

    if (player_dirty)
        draw_paddle(screen, PLAYER_X, g->player_y, page);
    if (cpu_dirty)
        draw_paddle(screen, CPU_X, g->cpu_y, page);
    gfx_draw_image_resource(screen, (u16)g->ball_x, (u8)g->ball_y, page,
                            ppong_images, RES_BALL, GFX_MASKED | GFX_VRAM_ONLY);
    remember_objects(screen, d, g);
}

static void draw_backgrounds(u8 page) {
    gfx_draw_image_resource(GFX_SCREEN_0, 0, 0, page, ppong_images, RES_BG0, GFX_OPAQUE);
    gfx_draw_image_resource(GFX_SCREEN_0, 0, 128, page, ppong_images, RES_BG1, GFX_OPAQUE);
    gfx_draw_image_resource(GFX_SCREEN_1, 0, 0, page, ppong_images, RES_BG0, GFX_OPAQUE);
    gfx_draw_image_resource(GFX_SCREEN_1, 0, 128, page, ppong_images, RES_BG1, GFX_OPAQUE);
}

static void run_game(u8 page, u8 music_page, u8 control) {
    game_state_t g;
    draw_state_t d;
    u8 hidden;
    u8 quit;
    u8 old_player_score;
    u8 old_cpu_score;

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
        video_vsync();
        gfx_flip();
        debug_border(BORDER_PLAYER);
        ay_pt3_play(music_page);
        debug_border(BORDER_AFTER);

        process_input(&g, &quit, control);
        if (quit) break;

        old_player_score = g.player_score;
        old_cpu_score = g.cpu_score;
        update_game(&g);

        hidden = gfx_get_active_screen() ^ 1;
        debug_border(BORDER_BLUE);
        if (old_player_score != g.player_score || old_cpu_score != g.cpu_score) {
            restore_score(GFX_SCREEN_0);
            restore_score(GFX_SCREEN_1);
            draw_score(GFX_SCREEN_0, page, &g);
            draw_score(GFX_SCREEN_1, page, &g);
        }
        render_objects(hidden, page, &d, &g);
        debug_border(BORDER_BLACK);
    }
    debug_border(BORDER_BLACK);
}

static u8 select_control(void) {
    u8 ch;

    dss_puts("\r\nControls:\r\n");
    dss_puts("1 - Joystick\r\n");
    dss_puts("2 - Port FE W/S\r\n");
    dss_puts("3 - DSS keyboard W/S/up/down\r\n");
    dss_puts("Select 1-3: ");

    for (;;) {
        ch = dss_waitkey();
        if (ch == '1') {
            dss_puts("Joystick\r\n");
            return CONTROL_JOYSTICK;
        }
        if (ch == '2') {
            dss_puts("Port FE\r\n");
            return CONTROL_FE;
        }
        if (ch == '3') {
            dss_puts("DSS keyboard\r\n");
            return CONTROL_DSS;
        }
    }
}

void main(void) {
    u8 block;
    u8 music_block;
    u8 control;

    block = 0xFF;
    music_block = 0xFF;
    dss_puts("PPONG demo\r\n");
    dss_puts("Loading PPONG.GFX...\r\n");
    block = dss_getmem_pages(PPONG_PAGE_COUNT);
    dss_puts("Loading PPONG.PT3...\r\n");
    music_block = dss_getmem_pages(MUSIC_PAGE_COUNT);
    if (block != 0xFF &&
        music_block != 0xFF &&
        gfx_load_resource_pages("PPONG.GFX", block, PPONG_PAGE_COUNT) > 0 &&
        asset_load_pages("PPONG.PT3", music_block, MUSIC_PAGE_COUNT) > 0) {
        control = select_control();
        dss_puts("ESC exits.\r\n");
        video_setmode(VMODE_320);
        show_screen0();
        video_setpal_range(0, PPONG_PALETTE_COUNT, ppong_palette);
        ay_pt3_init(music_block);
        run_game(block, music_block, control);
        ay_pt3_mute(music_block);
    } else {
        dss_puts("Resource load failed.\r\n");
        dss_waitkey();
    }

    if (music_block != 0xFF) {
        ay_pt3_mute(music_block);
        dss_freemem(music_block);
    }
    if (block != 0xFF)
        dss_freemem(block);
    video_setmode(VMODE_TEXT80);
}
