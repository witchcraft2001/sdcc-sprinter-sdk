#include <sprinter/bios.h>

u8 bios_emm_list(u8 block, u8 *pages) __naked {
    (void)block; (void)pages;
    __asm
        ; sdcccall(1): block -> A, pages -> DE
        push    ix
        ld      l, e
        ld      h, d
        ld      c, #0xC5
        rst     #0x08
        pop     ix
        ld      a, b
        ret
    __endasm;
}
