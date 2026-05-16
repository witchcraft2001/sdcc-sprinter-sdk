#include <sprinter/gfx.h>
#include <sprinter/assets.h>
#include <sprinter/dss.h>
#include <sprinter/bios.h>
#include <sprinter/ports.h>

#define GFX_PAGE_SIZE       0x4000U
#define GFX_SCREEN_WIDTH    320
#define GFX_SCREEN_HEIGHT   256
#define GFX_RESOURCE_MAX_PAGES 256

static u8 gfx_resource_block = 0xFF;
static u8 gfx_resource_page_count = 0;
/* Exposed to fast-path asm (see gfx_draw_image_full.s). */
u8 gfx_resource_phys[GFX_RESOURCE_MAX_PAGES];

void gfx_draw_sprite8_win0(u8 screen, u16 x, u8 y, const void *data, u8 flags) SPRINTER_NAKED_DECL;
void gfx_draw_sprite16_win0(u8 screen, u16 x, u8 y, const void *data, u8 flags) SPRINTER_NAKED_DECL;
void gfx_draw_sprite24_win0(u8 screen, u16 x, u8 y, const void *data, u8 flags) SPRINTER_NAKED_DECL;
void gfx_blit_line_accel(u8 screen, u16 x, u8 y, const void *src, u8 width) SPRINTER_NAKED_DECL;
void gfx_draw_image_full_cols(u8 screen, u8 page_delta, u8 flags) SPRINTER_NAKED_DECL;

static u8 gfx_page_from_flags_c(u8 flags) {
    u8 page;

    page = VRAM_PAGE_BASE;
    if (flags & GFX_MASKED) page += VRAM_TRANSPARENT;
    if (flags & GFX_VRAM_ONLY) page += VRAM_ONLY;
    return page;
}

static u8 gfx_get_resource_phys(u8 block, u8 page) {
    if (block != gfx_resource_block || page >= gfx_resource_page_count)
        return 0xFF;
    return gfx_resource_phys[page];
}

static u8 gfx_cache_resource_pages(u8 block, u8 page_count) {
    u8 count;

    count = bios_emm_list(block, gfx_resource_phys);
    if (count < page_count) return 0;
    gfx_resource_block = block;
    gfx_resource_page_count = count;
    return 1;
}

i16 gfx_load_resource_pages(const char *path, u8 first_page, u8 page_count) {
    i16 loaded;

    loaded = asset_load_pages(path, first_page, page_count);
    if (loaded < 0) return -1;
    if (!gfx_cache_resource_pages(first_page, (u8)loaded)) return -1;
    return loaded;
}

u8 gfx_select_resource_block(u8 base_page, u8 page_count) {
    return gfx_cache_resource_pages(base_page, page_count);
}

u8 gfx_draw_resource(u8 screen, u16 x, u8 y, u8 base_page,
                     const gfx_resource_t *resources, u8 id, u8 flags) {
    const gfx_resource_t *res;
    u8 old_page;
    u8 phys_page;
    u8 draw_flags;

    res = resources + id;
    draw_flags = flags | res->flags;

    phys_page = gfx_get_resource_phys(base_page, res->page_delta);
    if (phys_page == 0xFF) return 1;

    old_page = inp(PORT_WIN0);
    dss_di();
    outp(PORT_WIN0, phys_page);
    if (res->width == 8 && res->height == 8) {
        gfx_draw_sprite8_win0(screen, x, y, (const void *)res->offset, draw_flags);
    } else if (res->width == 16 && res->height == 16) {
        gfx_draw_sprite16_win0(screen, x, y, (const void *)res->offset, draw_flags);
    } else if (res->width == 24 && res->height == 24) {
        gfx_draw_sprite24_win0(screen, x, y, (const void *)res->offset, draw_flags);
    } else {
        outp(PORT_WIN0, old_page);
        dss_ei();
        return 1;
    }
    outp(PORT_WIN0, old_page);
    dss_ei();

    return 0;
}

void gfx_draw_image(u8 screen, u16 x, u8 y, u16 width, u8 height,
                    const void *data, u8 flags) {
    const u8 *src;
    u8 old_page;
    u16 rows;
    u16 row;
    u16 copy_width;
    u16 remaining;
    u16 done;
    u8 chunk;

    if (width == 0 || x >= GFX_SCREEN_WIDTH) return;

    rows = height ? height : GFX_SCREEN_HEIGHT;
    if ((u16)y + rows > GFX_SCREEN_HEIGHT) rows = GFX_SCREEN_HEIGHT - y;

    copy_width = width;
    if (copy_width > GFX_SCREEN_WIDTH - x)
        copy_width = GFX_SCREEN_WIDTH - x;

    src = (const u8 *)data;
    old_page = inp(PORT_WIN3);
    dss_di();
    outp(PORT_WIN3, gfx_page_from_flags_c(flags));

    for (row = 0; row < rows; row++) {
        remaining = copy_width;
        done = 0;
        while (remaining) {
            chunk = (remaining > 255) ? 255 : (u8)remaining;
            gfx_blit_line_accel(screen, x + done, (u8)((u16)y + row),
                                src + done, chunk);
            done += chunk;
            remaining -= chunk;
        }
        src += width;
    }

    outp(PORT_WIN3, old_page);
    video_safe_porty();
    dss_ei();
}

u8 gfx_draw_image_page(u8 screen, u16 x, u8 y, u8 base_page,
                       const gfx_image_t *image, u8 flags) {
    u8 old_src_page;
    u8 old_page;
    u8 phys_page;
    u8 draw_flags;
    u16 rows;
    u16 row;
    u16 copy_width;
    u16 remaining;
    u16 chunk;
    u16 src_off;
    u16 line_off;
    u8 line_page;
    u8 src_page;
    const void *src;
    u16 done;

    if (image->width == 0 || x >= GFX_SCREEN_WIDTH) return 1;
    if (image->page_delta >= gfx_resource_page_count) return 1;

    rows = image->height ? image->height : GFX_SCREEN_HEIGHT;
    if ((u16)y + rows > GFX_SCREEN_HEIGHT) rows = GFX_SCREEN_HEIGHT - y;

    copy_width = image->width;
    if (copy_width > GFX_SCREEN_WIDTH - x)
        copy_width = GFX_SCREEN_WIDTH - x;

    old_page = inp(PORT_WIN3);
    draw_flags = flags | image->flags;
    line_page = image->page_delta;
    line_off = image->offset;

    /* Full-screen 320x256 fast path: source data stored column-major with
     * 64 columns per page (256 bytes each), driven by the vertical-copy
     * accelerator command. Row-major sources fall through to the generic
     * line-by-line blit below.
     */
    if (x == 0 && y == 0 &&
        image->offset == 0 &&
        image->width == GFX_SCREEN_WIDTH &&
        image->height == 0 &&
        rows == GFX_SCREEN_HEIGHT &&
        (draw_flags & GFX_RESF_COLUMNS)) {
        if (gfx_get_resource_phys(base_page, image->page_delta) == 0xFF)
            return 1;
        gfx_draw_image_full_cols(screen, image->page_delta, draw_flags);
        return 0;
    }

    if (image->size <= GFX_PAGE_SIZE &&
        image->offset <= (u16)(GFX_PAGE_SIZE - image->size)) {
        phys_page = gfx_get_resource_phys(base_page, image->page_delta);
        if (phys_page == 0xFF) return 1;

        if (image->width == 8 && image->height == 8) {
            old_src_page = inp(PORT_WIN0);
            dss_di();
            outp(PORT_WIN0, phys_page);
            gfx_draw_sprite8_win0(screen, x, y, (const void *)image->offset, draw_flags);
            outp(PORT_WIN0, old_src_page);
            dss_ei();
            return 0;
        }
        if (image->width == 16 && image->height == 16) {
            old_src_page = inp(PORT_WIN0);
            dss_di();
            outp(PORT_WIN0, phys_page);
            gfx_draw_sprite16_win0(screen, x, y, (const void *)image->offset, draw_flags);
            outp(PORT_WIN0, old_src_page);
            dss_ei();
            return 0;
        }
        if (image->width == 24 && image->height == 24) {
            old_src_page = inp(PORT_WIN0);
            dss_di();
            outp(PORT_WIN0, phys_page);
            gfx_draw_sprite24_win0(screen, x, y, (const void *)image->offset, draw_flags);
            outp(PORT_WIN0, old_src_page);
            dss_ei();
            return 0;
        }
    }

    old_src_page = inp(PORT_WIN0);
    dss_di();
    outp(PORT_WIN3, gfx_page_from_flags_c(draw_flags));
    for (row = 0; row < rows; row++) {
        src_page = line_page;
        src_off = line_off;

        copy_width = image->width;
        if (copy_width > GFX_SCREEN_WIDTH - x)
            copy_width = GFX_SCREEN_WIDTH - x;

        remaining = copy_width;
        while (remaining) {
            phys_page = gfx_get_resource_phys(base_page, src_page);
            if (phys_page == 0xFF) {
                outp(PORT_WIN0, old_src_page);
                outp(PORT_WIN3, old_page);
                video_safe_porty();
                dss_ei();
                return 1;
            }
            outp(PORT_WIN0, phys_page);
            chunk = (u16)(GFX_PAGE_SIZE - src_off);
            if (chunk > remaining) chunk = remaining;
            if (chunk > 255) chunk = 255;
            src = (const void *)src_off;
            done = copy_width - remaining;
            gfx_blit_line_accel(screen, x + done, (u8)((u16)y + row),
                                src, (u8)chunk);
            src_off += chunk;
            if (src_off == GFX_PAGE_SIZE) {
                src_off = 0;
                src_page++;
            }
            remaining -= chunk;
        }

        line_off += image->width;
        while (line_off >= GFX_PAGE_SIZE) {
            line_off -= GFX_PAGE_SIZE;
            line_page++;
        }
    }

    outp(PORT_WIN0, old_src_page);
    outp(PORT_WIN3, old_page);
    video_safe_porty();
    dss_ei();
    return 0;
}

u8 gfx_draw_image_resource(u8 screen, u16 x, u8 y, u8 base_page,
                           const gfx_image_t *images, u8 id, u8 flags) {
    return gfx_draw_image_page(screen, x, y, base_page, images + id, flags);
}
