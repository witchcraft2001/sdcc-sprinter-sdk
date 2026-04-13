/**
 * mouse.h — ZX Sprinter Mouse API
 *
 * Mouse driver interface via RST #30 vector.
 */

#ifndef _SPRINTER_MOUSE_H
#define _SPRINTER_MOUSE_H

#include <sprinter/types.h>

/* Mouse function numbers (RST #30) */
#define MS_INIT         0x00    /* Initialize mouse */
#define MS_SHOW         0x01    /* Show cursor */
#define MS_HIDE         0x02    /* Hide cursor */
#define MS_STAT         0x03    /* Get mouse status */
#define MS_SETPOS       0x04    /* Set cursor position */
#define MS_XBOUND       0x05    /* Set X boundary */
#define MS_YBOUND       0x06    /* Set Y boundary */
#define MS_TEXTCUR      0x07    /* Set text cursor shape */
#define MS_SETCUR       0x08    /* Set sprite cursor */
#define MS_GETCUR       0x09    /* Get cursor info */
#define MS_GETSEN       0x0A    /* Get sensitivity */
#define MS_SETSEN       0x0B    /* Set sensitivity */
#define MS_HARD         0x0C    /* Get hardware info */
#define MS_VMOD         0x0D    /* Set video mode for mouse */
#define MS_REFRESH      0x0E    /* Refresh/redraw cursor */

/* Mouse button masks */
#define MS_BTN_LEFT     0x01
#define MS_BTN_RIGHT    0x02
#define MS_BTN_MIDDLE   0x04

/** Mouse state structure */
typedef struct {
    u8   buttons;       /* Button state (MS_BTN_xxx) */
    u16  x;             /* X position */
    u16  y;             /* Y position */
} mouse_state_t;


/* ===== Functions ===== */

/** Initialize mouse driver. Returns number of buttons or 0 if no mouse */
u8 mouse_init(void);

/** Show mouse cursor */
void mouse_show(void);

/** Hide mouse cursor */
void mouse_hide(void);

/** Get current mouse state */
void mouse_stat(mouse_state_t *state);

/** Set mouse cursor position */
void mouse_setpos(u16 x, u16 y);

/** Set X coordinate boundaries */
void mouse_xbound(u16 min_x, u16 max_x);

/** Set Y coordinate boundaries */
void mouse_ybound(u16 min_y, u16 max_y);

#endif /* _SPRINTER_MOUSE_H */
