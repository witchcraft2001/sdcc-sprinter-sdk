#include <sprinter/bios.h>

u8 bios_board_id(void) __naked {
    __asm
        push    ix
        ld      c, #0xED
        rst     #0x08
        pop     ix
        ret
    __endasm;
}
