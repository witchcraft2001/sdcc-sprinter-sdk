/**
 * ports.h — ZX Sprinter hardware port definitions
 *
 * I/O port addresses for direct hardware access.
 */

#ifndef _SPRINTER_PORTS_H
#define _SPRINTER_PORTS_H

/* ----- Memory Window Selectors ----- */
#define PORT_WIN0       0x82    /* Window 0: 0x0000-0x3FFF */
#define PORT_WIN1       0xA2    /* Window 1: 0x4000-0x7FFF */
#define PORT_WIN2       0xC2    /* Window 2: 0x8000-0xBFFF */
#define PORT_WIN3       0xE2    /* Window 3: 0xC000-0xFFFF */

/* ----- Video ----- */
#define PORT_GFXMODE    0xC3    /* Graphics mode: 00=ZX, 01=320x256x8, 10=640x256x4 */
#define PORT_RGMOD      0xC9    /* bit 0: display buffer select, bit 1: optional screen off */
#define PORT_PAL_ADDR   0x89    /* Palette address (PORT_Y) */
#define PORT_PAL_DATA   0x89    /* Palette data */
#define PORT_CBL_DIR    0x004E  /* CBL/Covox-Blaster control, 16-bit port: use OUT (C),A */

/* ----- VRAM Pages ----- */
/* Page format: 0x5X where X = %TSPP
 *   T = Transparency (skip 0xFF bytes)
 *   S = VRAM-only write
 *   P = Buffer select (0-3)
 */
#define VRAM_PAGE_BASE  0x50

/* ----- Sound ----- */
#define PORT_AY_ADDR    0x8C    /* AY-3-8910 register address */
#define PORT_AY_WRITE   0x8D    /* AY-3-8910 data write */
#define PORT_AY_READ    0x8E    /* AY-3-8910 data read */
#define PORT_COVOX      0x88    /* Covox DAC output */
#define PORT_COVOX_MODE 0x89    /* Covox mode */

/* ----- Keyboard & Mouse ----- */
#define PORT_KEYB       0xFE    /* ZX keyboard + border color */
#define PORT_KEMPSTON   0x1F    /* Kempston joystick */
#define PORT_MOUSE_BTN  0x58    /* Mouse buttons */

/* ----- CTC Timers ----- */
#define PORT_CTC0       0x10
#define PORT_CTC1       0x11
#define PORT_CTC2       0x12
#define PORT_CTC3       0x13

/* ----- SIO (PS/2) ----- */
#define PORT_SIO_A_DATA 0x18
#define PORT_SIO_A_CMD  0x19

/* ----- ROM/Flash ----- */
#define PORT_ROM_RG     0x8F

/* ----- DMA / Blitter ----- */
#define PORT_DMA_SRC_L  0x84
#define PORT_DMA_SRC_H  0x85
#define PORT_DMA_DST_L  0x86
#define PORT_DMA_DST_H  0x87

#endif /* _SPRINTER_PORTS_H */
