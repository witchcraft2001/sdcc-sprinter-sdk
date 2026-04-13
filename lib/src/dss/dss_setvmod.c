#include <sprinter/dss.h>

u8 dss_setvmod(u8 mode, u8 page) __naked {
    (void)mode; (void)page;
    __asm
        ; sdcccall(1): mode -> A, page -> L
        ld      b, l            ; B = page
        push    ix
        ld      c, #0x50
        rst     #0x10
        pop     ix
        jr      c, _dss_svm_err
        xor     a
        ret
_dss_svm_err:
        ret
    __endasm;
}
