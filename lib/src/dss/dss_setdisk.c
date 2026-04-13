#include <sprinter/dss.h>

void dss_setdisk(u8 disk) __naked {
    __asm
        push    ix
        ld      c, #0x01
        rst     #0x10
        pop     ix
        ret
    __endasm;
}
