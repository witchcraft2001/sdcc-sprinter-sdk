#include <sprinter/dss.h>

void dss_ei(void) __naked {
    __asm
        ei
        ret
    __endasm;
}
