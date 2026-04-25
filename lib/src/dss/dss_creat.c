#include <sprinter/dss.h>

i16 dss_creat(const char *path) __naked {
#if defined(SDCC) && (SDCC < 300)
    __asm
        push    ix
        ld      a, #0x20
        ld      c, #0x0A
        rst     #0x10
        pop     ix
        jr      c, _dss_creat_err_290
        ld      l, a
        ld      h, #0
        ret
_dss_creat_err_290:
        ld      hl, #0xFFFF
        ret
    __endasm;
#else
    __asm
        push    ix
        ld      a, #0x20        ; attribute: archive
        ld      c, #0x0A
        rst     #0x10
        pop     ix
        jr      c, _dss_creat_err
        ld      e, a
        ld      d, #0
        ret
_dss_creat_err:
        ld      de, #0xFFFF
        ret
    __endasm;
#endif
}
