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
#define BIOS_PUTPIXEL   0xA1    /* Put pixel: HL=X, DE=Y, B=color */
#define BIOS_SETPAL     0xA4    /* Set palette: HL=index, D=R, E=G, A=B */

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
#define BIOS_DRV_RESET       0x51    /* Reset disk controller */
#define BIOS_DRV_READ_LONG   0x52    /* Long-form sector read */
#define BIOS_DRV_WRITE_LONG  0x53    /* Long-form sector write */
#define BIOS_DRV_VERIFY      0x54    /* Verify sectors */
#define BIOS_DRV_READ        0x55    /* Short sector read, 1..255 sectors */
#define BIOS_DRV_WRITE       0x56    /* Short sector write, 1..255 sectors */
#define BIOS_DRV_DETECT      0x57    /* Detect disk */
#define BIOS_DRV_GET_PAR     0x58    /* Get disk parameters */
#define BIOS_DRV_SET_PAR     0x59    /* Set disk parameters */
#define BIOS_DRV_VERSION     0x5A    /* Disk driver specification version */
#define BIOS_DRV_EXTENDED    0x5E    /* Extended driver calls */
#define BIOS_DRV_LIST        0x5F    /* List drives */

#define BIOS_DISK_RESET      BIOS_DRV_RESET
#define BIOS_DISK_READ       BIOS_DRV_READ
#define BIOS_DISK_WRITE      BIOS_DRV_WRITE
#define BIOS_DISK_DET        BIOS_DRV_DETECT
#define BIOS_DISK_PAR        BIOS_DRV_GET_PAR

/* Service */
#define BIOS_BOARD_ID   0xED    /* Get board ID */
#define BIOS_VERSION    0xEE    /* Get BIOS version */
#define BIOS_SP_TYPE    0xEF    /* Get Sprinter type */


/* ===== Functions ===== */

/** Set a single palette entry via BIOS #A4.
 *  index: color index (0-255)
 *  r, g, b: color components (0-63 each, 6-bit)
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

/** Read 1..255 sectors via BIOS DRV_READ. Returns 0 or BIOS error code. */
u8 bios_drv_read(u8 disk, u32 lba, void *dst, u8 count);

/** Write 1..255 sectors via BIOS DRV_WRITE. Returns 0 or BIOS error code. */
u8 bios_drv_write(u8 disk, u32 lba, const void *src, u8 count);

/** Read I/O port */
u8 inp(u16 port);

/** Write I/O port */
void outp(u16 port, u8 value);

#endif /* _SPRINTER_BIOS_H */
