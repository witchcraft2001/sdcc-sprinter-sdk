#include <sprinter/dss.h>

void dss_di(void) __naked {
    __asm
        di
        ret
    __endasm;
}
