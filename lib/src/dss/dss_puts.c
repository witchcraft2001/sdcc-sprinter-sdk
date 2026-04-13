#include <sprinter/dss.h>

void dss_puts(const char *str) __naked {
    __asm
        push    ix
        ld      c, #0x5C
        rst     #0x10
        pop     ix
        ret
    __endasm;
}
