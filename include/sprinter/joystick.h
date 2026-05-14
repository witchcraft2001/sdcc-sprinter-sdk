/**
 * joystick.h - Kempston-compatible joystick polling.
 *
 * The bit masks match the original ZX Sprinter SDK joystick() API.
 */

#ifndef _SPRINTER_JOYSTICK_H
#define _SPRINTER_JOYSTICK_H

#include <sprinter/types.h>

#define JOY_RIGHT   0x01
#define JOY_LEFT    0x02
#define JOY_DOWN    0x04
#define JOY_UP      0x08
#define JOY_FIRE    0x10
#define JOY_START   0x20
#define JOY_ESC     0x80

/** Read current Kempston-compatible joystick state as JOY_* bits. */
u8 joystick(void) SPRINTER_NAKED_DECL;

#endif /* _SPRINTER_JOYSTICK_H */
