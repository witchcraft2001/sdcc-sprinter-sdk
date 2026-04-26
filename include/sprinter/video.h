/**
 * video.h — ZX Sprinter Video API
 *
 * Video mode control and VRAM access.
 */

#ifndef _SPRINTER_VIDEO_H
#define _SPRINTER_VIDEO_H

#include <sprinter/types.h>

/* Video modes for DSS SETVMOD (#50): A = mode, B = page
 * Bit 7: 0 = text, 1 = graphics (from DSS VIDEO.ASM) */
#define VMODE_TEXT40    0x02    /* Text 40x32 */
#define VMODE_TEXT80    0x03    /* Text 80x32 */
#define VMODE_ZX        0x03    /* Alias for default text mode */
#define VMODE_320_16    0x80    /* 320x256, 16 colors */
#define VMODE_320       0x81    /* 320x256, 256 colors (8bpp) */
#define VMODE_640_16    0x82    /* 640x256, 16 colors */
#define VMODE_640       0x82    /* Alias */

/* Screen dimensions */
#define SCREEN_W_320    320
#define SCREEN_H_320    256
#define SCREEN_W_640    640
#define SCREEN_H_640    256

/* Text mode dimensions */
#define TEXT_COLS       80
#define TEXT_ROWS       32

/* VRAM page flags (OR with page number) */
#define VRAM_TRANSPARENT    0x08    /* Skip 0xFF bytes */
#define VRAM_ONLY           0x04    /* Write to VRAM only */

/** 6-bit RGB palette entry, matching Sprinter hardware range. */
typedef struct video_rgb6 {
    u8 r;
    u8 g;
    u8 b;
} video_rgb6_t;

/** 8-bit RGB palette entry, scaled to hardware range by video_* APIs. */
typedef struct video_rgb8 {
    u8 r;
    u8 g;
    u8 b;
} video_rgb8_t;

/** Set graphics video mode */
void video_setmode(u8 mode) SPRINTER_NAKED_DECL;

/** Get current video mode */
u8 video_getmode(void) SPRINTER_NAKED_DECL;

/** Swap display buffers (double buffering via RGMOD port) */
void video_swap(void) SPRINTER_NAKED_DECL;

/** Enable the hardware sync source used by video_vsync() */
void video_sync_enable(void) SPRINTER_NAKED_DECL;

/** Disable the hardware sync source used by video_vsync() */
void video_sync_disable(void) SPRINTER_NAKED_DECL;

/** Wait for the start of a frame using Sprinter CBL/#FFFE bit 5 when available */
void video_vsync(void) SPRINTER_NAKED_DECL;

/** Set palette entry.
 *  index: color index
 *  r, g, b: 0-255 each
 */
void video_setpal(u8 index, u8 r, u8 g, u8 b);

/** Set a range of palette entries using 6-bit RGB components.
 *  first: first palette index.
 *  count: number of entries, 1..256. The range must not wrap past 255.
 *  colors: count entries in RGB order.
 */
void video_setpal_range(u8 first, u16 count, const video_rgb6_t *colors);

/** Set a range of palette entries using 8-bit RGB components.
 *  Components are scaled down to Sprinter's 6-bit hardware palette.
 */
void video_setpal_range8(u8 first, u16 count, const video_rgb8_t *colors);

/** Load BIOS GRAF preset palette. */
void video_setpal_graf(void) SPRINTER_NAKED_DECL;

/** Map VRAM page into memory window.
 *  win: memory window (0-3)
 *  page: VRAM page number (0x50-0x5F)
 */
void video_mapvram(u8 win, u8 page) SPRINTER_NAKED_DECL;

/** Reset PORT_Y to safe zone (0xC0).
 *  MUST be called after finishing all pixel drawing to prevent
 *  VRAM corruption from code/data accesses. */
void video_safe_porty(void) SPRINTER_NAKED_DECL;

#endif /* _SPRINTER_VIDEO_H */
