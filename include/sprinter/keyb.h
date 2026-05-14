/**
 * keyb.h - Direct ZX keyboard matrix polling for realtime input.
 *
 * This bypasses the DSS keyboard event buffer. It is intended for games and
 * other frame-polled code that needs current held-key state without typematic
 * repeat delays.
 */

#ifndef _SPRINTER_KEYB_H
#define _SPRINTER_KEYB_H

#include <sprinter/types.h>

/* Matrix rows. Pass one of these to keyb_read_row(). */
#define KEYB_ROW_SHIFT  0xFE    /* Caps Shift, Z, X, C, V */
#define KEYB_ROW_ASDFG  0xFD    /* A, S, D, F, G */
#define KEYB_ROW_QWERT  0xFB    /* Q, W, E, R, T */
#define KEYB_ROW_12345  0xF7    /* 1, 2, 3, 4, 5 */
#define KEYB_ROW_09876  0xEF    /* 0, 9, 8, 7, 6 */
#define KEYB_ROW_POIUY  0xDF    /* P, O, I, U, Y */
#define KEYB_ROW_ENTER  0xBF    /* Enter, L, K, J, H */
#define KEYB_ROW_SPACE  0x7F    /* Space, Symbol Shift, M, N, B */

#define KEYB_CODE(row, bit)  ((((u16)(row)) << 8) | (u16)(bit))

#define KEYB_CAPS_SHIFT KEYB_CODE(KEYB_ROW_SHIFT, 0)
#define KEYB_Z          KEYB_CODE(KEYB_ROW_SHIFT, 1)
#define KEYB_X          KEYB_CODE(KEYB_ROW_SHIFT, 2)
#define KEYB_C          KEYB_CODE(KEYB_ROW_SHIFT, 3)
#define KEYB_V          KEYB_CODE(KEYB_ROW_SHIFT, 4)

#define KEYB_A          KEYB_CODE(KEYB_ROW_ASDFG, 0)
#define KEYB_S          KEYB_CODE(KEYB_ROW_ASDFG, 1)
#define KEYB_D          KEYB_CODE(KEYB_ROW_ASDFG, 2)
#define KEYB_F          KEYB_CODE(KEYB_ROW_ASDFG, 3)
#define KEYB_G          KEYB_CODE(KEYB_ROW_ASDFG, 4)

#define KEYB_Q          KEYB_CODE(KEYB_ROW_QWERT, 0)
#define KEYB_W          KEYB_CODE(KEYB_ROW_QWERT, 1)
#define KEYB_E          KEYB_CODE(KEYB_ROW_QWERT, 2)
#define KEYB_R          KEYB_CODE(KEYB_ROW_QWERT, 3)
#define KEYB_T          KEYB_CODE(KEYB_ROW_QWERT, 4)

#define KEYB_1          KEYB_CODE(KEYB_ROW_12345, 0)
#define KEYB_2          KEYB_CODE(KEYB_ROW_12345, 1)
#define KEYB_3          KEYB_CODE(KEYB_ROW_12345, 2)
#define KEYB_4          KEYB_CODE(KEYB_ROW_12345, 3)
#define KEYB_5          KEYB_CODE(KEYB_ROW_12345, 4)

#define KEYB_0          KEYB_CODE(KEYB_ROW_09876, 0)
#define KEYB_9          KEYB_CODE(KEYB_ROW_09876, 1)
#define KEYB_8          KEYB_CODE(KEYB_ROW_09876, 2)
#define KEYB_7          KEYB_CODE(KEYB_ROW_09876, 3)
#define KEYB_6          KEYB_CODE(KEYB_ROW_09876, 4)

#define KEYB_P          KEYB_CODE(KEYB_ROW_POIUY, 0)
#define KEYB_O          KEYB_CODE(KEYB_ROW_POIUY, 1)
#define KEYB_I          KEYB_CODE(KEYB_ROW_POIUY, 2)
#define KEYB_U          KEYB_CODE(KEYB_ROW_POIUY, 3)
#define KEYB_Y          KEYB_CODE(KEYB_ROW_POIUY, 4)

#define KEYB_ENTER      KEYB_CODE(KEYB_ROW_ENTER, 0)
#define KEYB_L          KEYB_CODE(KEYB_ROW_ENTER, 1)
#define KEYB_K          KEYB_CODE(KEYB_ROW_ENTER, 2)
#define KEYB_J          KEYB_CODE(KEYB_ROW_ENTER, 3)
#define KEYB_H          KEYB_CODE(KEYB_ROW_ENTER, 4)

#define KEYB_SPACE      KEYB_CODE(KEYB_ROW_SPACE, 0)
#define KEYB_SYM_SHIFT  KEYB_CODE(KEYB_ROW_SPACE, 1)
#define KEYB_M          KEYB_CODE(KEYB_ROW_SPACE, 2)
#define KEYB_N          KEYB_CODE(KEYB_ROW_SPACE, 3)
#define KEYB_B          KEYB_CODE(KEYB_ROW_SPACE, 4)

/** Read a ZX matrix row. Low five bits are returned as 1 = key pressed. */
u8 keyb_read_row(u8 row) SPRINTER_NAKED_DECL;

/** Return true when the KEYB_* key is currently held. */
bool keyb_pressed(u16 key) SPRINTER_NAKED_DECL;

#endif /* _SPRINTER_KEYB_H */
