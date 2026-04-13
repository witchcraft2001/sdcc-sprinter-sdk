/**
 * video.h — ZX Sprinter Video API
 *
 * Video mode control and VRAM access.
 */

#ifndef _SPRINTER_VIDEO_H
#define _SPRINTER_VIDEO_H

#include <sprinter/types.h>

/* Video modes (PORT_GFXMODE bits 0-1) */
#define VMODE_ZX        0x00    /* ZX Spectrum compatible (256x192) */
#define VMODE_320       0x01    /* 320x256, 256 colors (8bpp) */
#define VMODE_640       0x02    /* 640x256, 16 colors (4bpp) */

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

/** Set graphics video mode */
void video_setmode(u8 mode);

/** Get current video mode */
u8 video_getmode(void);

/** Swap display buffers (double buffering via RGMOD port) */
void video_swap(void);

/** Wait for vertical sync */
void video_vsync(void);

/** Set palette entry (2-bit RGB).
 *  index: color index
 *  r, g, b: 0-3 each
 */
void video_setpal(u8 index, u8 r, u8 g, u8 b);

/** Map VRAM page into memory window.
 *  win: memory window (0-3)
 *  page: VRAM page number (0x50-0x5F)
 */
void video_mapvram(u8 win, u8 page);

#endif /* _SPRINTER_VIDEO_H */
