#include <sprinter/gfx.h>
#include <sprinter/bios.h>
#include <sprinter/ports.h>
#include <string.h>

#define GFX_W 320
#define GFX_H 256
#define GFX_SCREEN_OFFSET 320

static u8 gfx_page_from_flags_c(u8 flags) {
    u8 page;

    page = VRAM_PAGE_BASE;
    if (flags & GFX_MASKED) page += VRAM_TRANSPARENT;
    if (flags & GFX_VRAM_ONLY) page += VRAM_ONLY;
    return page;
}

static volatile u8 *gfx_vram_addr(u8 screen, u16 x) {
    return (volatile u8 *)(0xC000 + x + ((screen & 1) ? GFX_SCREEN_OFFSET : 0));
}

static u8 gfx_begin_draw(u8 flags) {
    u8 old_page;

    old_page = inp(PORT_WIN3);
    outp(PORT_WIN3, gfx_page_from_flags_c(flags));
    return old_page;
}

static void gfx_end_draw(u8 old_page) {
    outp(PORT_WIN3, old_page);
    video_safe_porty();
}

static i16 gfx_abs16(i16 v) {
    return (v < 0) ? -v : v;
}

static void gfx_raw_pixel(u8 screen, i16 x, i16 y, u8 color) {
    volatile u8 *vram;

    if (x < 0 || x >= GFX_W || y < 0 || y >= GFX_H) return;

    outp(PORT_PAL_ADDR, (u8)y);
    vram = gfx_vram_addr(screen, (u16)x);
    *vram = color;
}

static void gfx_raw_brush(u8 screen, i16 x, i16 y, u8 thickness, u8 color) {
    i16 ox;
    i16 oy;
    i16 half;

    if (thickness <= 1) {
        gfx_raw_pixel(screen, x, y, color);
        return;
    }

    half = thickness >> 1;
    for (oy = 0; oy < thickness; oy++) {
        for (ox = 0; ox < thickness; ox++) {
            gfx_raw_pixel(screen, x + ox - half, y + oy - half, color);
        }
    }
}

static void gfx_raw_circle_points(u8 screen, i16 cx, i16 cy, i16 x, i16 y, u8 color) {
    gfx_raw_pixel(screen, cx + x, cy + y, color);
    gfx_raw_pixel(screen, cx + y, cy + x, color);
    gfx_raw_pixel(screen, cx - y, cy + x, color);
    gfx_raw_pixel(screen, cx - x, cy + y, color);
    gfx_raw_pixel(screen, cx - x, cy - y, color);
    gfx_raw_pixel(screen, cx - y, cy - x, color);
    gfx_raw_pixel(screen, cx + y, cy - x, color);
    gfx_raw_pixel(screen, cx + x, cy - y, color);
}

void gfx_draw_pixel(u8 screen, u16 x, u8 y, u8 color, u8 flags) {
    u8 old_page;

    if (x >= GFX_W) return;

    old_page = gfx_begin_draw(flags);
    gfx_raw_pixel(screen, (i16)x, (i16)y, color);
    gfx_end_draw(old_page);
}

void gfx_draw_hline(u8 screen, u16 x, u8 y, u16 width, u8 color, u8 flags) {
    u8 old_page;
    volatile u8 *vram;

    if (width == 0 || x >= GFX_W) return;
    if (x + width > GFX_W) width = GFX_W - x;

    old_page = gfx_begin_draw(flags);
    outp(PORT_PAL_ADDR, y);
    vram = gfx_vram_addr(screen, x);
    memset((void *)vram, color, width);
    gfx_end_draw(old_page);
}

void gfx_draw_vline(u8 screen, u16 x, u8 y, u8 height, u8 color, u8 flags) {
    u8 old_page;
    u16 rows;
    u16 row;

    if (x >= GFX_W) return;

    rows = height ? height : 256;
    if ((u16)y + rows > GFX_H) rows = GFX_H - y;

    old_page = gfx_begin_draw(flags);
    for (row = 0; row < rows; row++) {
        gfx_raw_pixel(screen, (i16)x, (i16)((u16)y + row), color);
    }
    gfx_end_draw(old_page);
}

void gfx_draw_line_thick(u8 screen, u16 x0, u8 y0, u16 x1, u8 y1,
                         u8 thickness, u8 color, u8 flags) {
    u8 old_page;
    i16 ax;
    i16 ay;
    i16 bx;
    i16 by;
    i16 dx;
    i16 dy;
    i16 sx;
    i16 sy;
    i16 err;
    i16 e2;

    if (thickness == 0) return;

    ax = (i16)x0;
    ay = (i16)y0;
    bx = (i16)x1;
    by = (i16)y1;
    dx = gfx_abs16(bx - ax);
    dy = -gfx_abs16(by - ay);
    sx = (ax < bx) ? 1 : -1;
    sy = (ay < by) ? 1 : -1;
    err = dx + dy;

    old_page = gfx_begin_draw(flags);
    for (;;) {
        gfx_raw_brush(screen, ax, ay, thickness, color);
        if (ax == bx && ay == by) break;
        e2 = err << 1;
        if (e2 >= dy) {
            err += dy;
            ax += sx;
        }
        if (e2 <= dx) {
            err += dx;
            ay += sy;
        }
    }
    gfx_end_draw(old_page);
}

void gfx_draw_line(u8 screen, u16 x0, u8 y0, u16 x1, u8 y1, u8 color, u8 flags) {
    gfx_draw_line_thick(screen, x0, y0, x1, y1, 1, color, flags);
}

void gfx_fill_rect(u8 screen, u16 x, u8 y, u16 width, u8 height, u8 color, u8 flags) {
    u8 old_page;
    u16 rows;
    u16 row;
    volatile u8 *vram;

    if (width == 0 || x >= GFX_W) return;
    if (x + width > GFX_W) width = GFX_W - x;

    rows = height ? height : 256;
    if ((u16)y + rows > GFX_H) rows = GFX_H - y;

    old_page = gfx_begin_draw(flags);
    for (row = 0; row < rows; row++) {
        outp(PORT_PAL_ADDR, (u8)((u16)y + row));
        vram = gfx_vram_addr(screen, x);
        memset((void *)vram, color, width);
    }
    gfx_end_draw(old_page);
}

void gfx_draw_rect(u8 screen, u16 x, u8 y, u16 width, u8 height, u8 color, u8 flags) {
    gfx_draw_rect_thick(screen, x, y, width, height, 1, color, flags);
}

void gfx_draw_rect_thick(u8 screen, u16 x, u8 y, u16 width, u8 height,
                         u8 thickness, u8 color, u8 flags) {
    u8 i;

    if (thickness == 0 || width == 0 || height == 0) return;

    for (i = 0; i < thickness; i++) {
        if (width <= (u16)i * 2 || height <= (u8)(i * 2)) break;
        gfx_draw_hline(screen, x + i, y + i, width - (u16)i * 2, color, flags);
        gfx_draw_hline(screen, x + i, y + height - 1 - i, width - (u16)i * 2, color, flags);
        gfx_draw_vline(screen, x + i, y + i, height - i * 2, color, flags);
        gfx_draw_vline(screen, x + width - 1 - i, y + i, height - i * 2, color, flags);
    }
}

void gfx_draw_circle(u8 screen, u16 cx, u8 cy, u8 radius, u8 color, u8 flags) {
    u8 old_page;
    i16 x;
    i16 y;
    i16 err;

    old_page = gfx_begin_draw(flags);
    x = radius;
    y = 0;
    err = 0;

    while (x >= y) {
        gfx_raw_circle_points(screen, (i16)cx, (i16)cy, x, y, color);
        if (err <= 0) {
            y++;
            err += (y << 1) + 1;
        }
        if (err > 0) {
            x--;
            err -= (x << 1) + 1;
        }
    }
    gfx_end_draw(old_page);
}
