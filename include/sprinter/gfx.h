/**
 * gfx.h - ZX Sprinter 320x256 graphics helper library
 *
 * This header belongs to the optional gfx.lib archive. Include and link it
 * explicitly when a program needs sprites, blits, or future graphics
 * primitives.
 */

#ifndef _SPRINTER_GFX_H
#define _SPRINTER_GFX_H

#include <sprinter/types.h>
#include <sprinter/video.h>

#define GFX_SCREEN_0        0
#define GFX_SCREEN_1        1

#define GFX_OPAQUE          0x00
#define GFX_MASKED          0x01    /* Skip color 0xFF via VRAM transparent mode */
#define GFX_VRAM_ONLY       0x02    /* Do not update shadow RAM */

#define GFX_TRANSPARENT_COLOR 0xFF

#define GFX_RESF_MASKED     GFX_MASKED

typedef struct gfx_resource {
    u8  page_delta;
    u16 offset;
    u16 size;
    u8  width;
    u8  height;
    u8  flags;
} gfx_resource_t;

void gfx_set_screen(u8 screen);
u8   gfx_get_screen(void);
u8   gfx_get_active_screen(void) SPRINTER_NAKED_DECL;
void gfx_flip(void) SPRINTER_NAKED_DECL;

void gfx_draw_sprite8(u8 screen, u16 x, u8 y, const void *data, u8 flags) SPRINTER_NAKED_DECL;
void gfx_draw_sprite16(u8 screen, u16 x, u8 y, const void *data, u8 flags) SPRINTER_NAKED_DECL;
void gfx_draw_sprite24(u8 screen, u16 x, u8 y, const void *data, u8 flags) SPRINTER_NAKED_DECL;

void gfx_restore_rect(u8 screen, u16 x, u8 y, u8 width, u8 height) SPRINTER_NAKED_DECL;
void gfx_restore_sprite8(u8 screen, u16 x, u8 y);
void gfx_restore_sprite16(u8 screen, u16 x, u8 y);
void gfx_restore_sprite24(u8 screen, u16 x, u8 y);

void gfx_copy_rect(u8 dst_screen, u8 src_screen, u16 x, u8 y, u8 width, u8 height) SPRINTER_NAKED_DECL;
void gfx_copy_screen(u8 dst_screen, u8 src_screen);

i16 gfx_load_resource_pages(const char *path, u8 first_page, u8 page_count);
u8  gfx_draw_resource(u8 screen, u16 x, u8 y, u8 base_page,
                      const gfx_resource_t *resources, u8 id, u8 flags);

#endif /* _SPRINTER_GFX_H */
