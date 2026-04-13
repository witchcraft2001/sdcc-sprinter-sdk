/**
 * bios.h — ZX Sprinter BIOS API
 *
 * Low-level hardware access via RST #08 vector.
 * Use DSS calls (dss.h) for normal programming.
 * BIOS calls are for direct hardware access only.
 */

#ifndef _SPRINTER_BIOS_H
#define _SPRINTER_BIOS_H

#include <sprinter/types.h>

/* ===== BIOS Function Numbers ===== */

/* Print / Text */
#define BIOS_PRINTCHAR  0x81    /* Print character at cursor */
#define BIOS_PRINTSYM   0x82    /* Print symbol (no control codes) */
#define BIOS_SETPLACE   0x83    /* Set text cursor position */
#define BIOS_PRINTLINE  0x84    /* Print string */
#define BIOS_CLEARWIN   0x85    /* Clear text window */
#define BIOS_SCROLL     0x86    /* Scroll text window */

/* Graphics */
#define BIOS_PUTPIXEL   0xA0    /* Put pixel */
#define BIOS_SETPAL     0xA1    /* Set palette entry */

/* Windows */
#define BIOS_OPENWIN    0xB0    /* Open text window */
#define BIOS_CLOSEWIN   0xB1    /* Close text window */
#define BIOS_COPYWIN    0xB2    /* Copy text window */
#define BIOS_GETSYM     0xB3    /* Get character at position */
#define BIOS_PUTSYM     0xB4    /* Put character at position */
#define BIOS_LOADFONT   0xB8    /* Load font */

/* EMM (Extended Memory) */
#define BIOS_EMM_INFO   0xC0    /* Get memory info */
#define BIOS_EMM_INIT   0xC1    /* Initialize EMM */
#define BIOS_EMM_ALLOC  0xC2    /* Allocate page */
#define BIOS_EMM_FREE   0xC3    /* Free page */
#define BIOS_EMM_PHYS   0xC4    /* Get physical page */
#define BIOS_EMM_LIST   0xC5    /* Get page list */
#define BIOS_EMM_WIN    0xC6    /* Get window mapping */
#define BIOS_EMM_NEXT   0xC7    /* Get next page */

/* Disk (low-level) */
#define BIOS_DISK_RESET 0x51    /* Reset disk controller */
#define BIOS_DISK_READ  0x53    /* Read sectors */
#define BIOS_DISK_WRITE 0x54    /* Write sectors */
#define BIOS_DISK_DET   0x57    /* Detect disk */
#define BIOS_DISK_PAR   0x58    /* Get disk parameters */

/* Service */
#define BIOS_BOARD_ID   0xED    /* Get board ID */
#define BIOS_VERSION    0xEE    /* Get BIOS version */
#define BIOS_SP_TYPE    0xEF    /* Get Sprinter type */


/* ===== Functions ===== */

/** Set palette color.
 *  index: color index (0-63 in 320x256 mode, 0-15 in 640x256 mode)
 *  r, g, b: color components (0-3, 2-bit each)
 */
void bios_setpal(u8 index, u8 r, u8 g, u8 b);

/** Put pixel in graphics mode.
 *  x: 0-319 (or 0-639)
 *  y: 0-255
 *  color: palette index
 */
void bios_putpixel(u16 x, u8 y, u8 color);

/** Get BIOS version. Returns version in BCD format */
u16 bios_version(void);

/** Get board/Sprinter type */
u8 bios_board_id(void);

/** Read I/O port */
u8 inp(u16 port);

/** Write I/O port */
void outp(u16 port, u8 value);

#endif /* _SPRINTER_BIOS_H */
