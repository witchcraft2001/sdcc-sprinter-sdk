#include <sprinter/bios.h>

u16 bios_version(void) __naked {
    __asm
        push    ix
        ld      c, #0xEE
        rst     #0x08
        pop     ix
        ld      e, a
        ld      d, b
        ret
    __endasm;
}
