#include <sprinter/bios.h>

u8 bios_board_id(void) __naked {
#if defined(SDCC) && (SDCC < 300)
    __asm
        push    ix
        ld      c, #0xED
        rst     #0x08
        pop     ix
        ld      l, a
        ld      h, #0
        ret
    __endasm;
#else
    __asm
        push    ix
        ld      c, #0xED
        rst     #0x08
        pop     ix
        ret
    __endasm;
#endif
}
