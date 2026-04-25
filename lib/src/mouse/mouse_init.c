#include <sprinter/mouse.h>

u8 mouse_init(void) __naked {
    /* Returns: CF=0 mouse present (A=1), CF=1 no mouse (A=0) */
#if defined(SDCC) && (SDCC < 300)
    __asm
        push    ix
        ld      c, #0x00
        rst     #0x30
        pop     ix
        ld      hl, #0x0000
        jr      c, _mouse_init_no_290
        inc     l
_mouse_init_no_290:
        ret
    __endasm;
#else
    __asm
        push    ix
        ld      c, #0x00
        rst     #0x30
        pop     ix
        jr      c, _mouse_init_no
        ld      a, #1           ; mouse present
        ret
_mouse_init_no:
        xor     a               ; no mouse
        ret
    __endasm;
#endif
}
