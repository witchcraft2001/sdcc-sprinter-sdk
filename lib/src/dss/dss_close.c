#include <sprinter/dss.h>

u8 dss_close(u8 fd) __naked {
    __asm
        push    ix
        ld      c, #0x12
        rst     #0x10
        pop     ix
        ret
    __endasm;
}
