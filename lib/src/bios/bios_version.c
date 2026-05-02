#include <sprinter/bios.h>

u16 bios_version(void) __naked {
    __asm
        push    ix
        ld      c, #0x5A
        rst     #0x08
        pop     ix
        ret
    __endasm;
}
