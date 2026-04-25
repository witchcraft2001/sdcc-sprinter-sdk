#include <sprinter/dss.h>

i16 dss_open(const char *path, u8 mode) __naked {
    (void)path; (void)mode;
#if defined(SDCC) && (SDCC < 300)
    __asm
        push    ix
        ld      iy, #4
        add     iy, sp
        ld      l, 0 (iy)
        ld      h, 1 (iy)
        ld      a, 2 (iy)
        inc     a               ; O_RDONLY(0)->1, O_WRONLY(1)->2, O_RDWR(2)->3
        cp      #3
        jr      nz, _dss_open_go_290
        xor     a               ; O_RDWR: 3->0 (DSS R/W mode)
_dss_open_go_290:
        ld      c, #0x11
        rst     #0x10
        pop     ix
        jr      c, _dss_open_err_290
        ld      l, a
        ld      h, #0
        ret
_dss_open_err_290:
        ld      hl, #0xFFFF
        ret
    __endasm;
#else
    __asm
        push    ix
        ld      iy, #4
        add     iy, sp
        ld      a, 0 (iy)      ; mode from stack
        inc     a               ; O_RDONLY(0)->1, O_WRONLY(1)->2, O_RDWR(2)->3
        cp      #3
        jr      nz, _dss_open_go
        xor     a               ; O_RDWR: 3->0 (DSS R/W mode)
_dss_open_go:
        ld      c, #0x11
        rst     #0x10
        pop     ix
        jr      c, _dss_open_err
        ld      e, a
        ld      d, #0
        ; clean 1 byte of stack param, return DE
        pop     iy
        inc     sp
        jp      (iy)
_dss_open_err:
        ld      de, #0xFFFF
        pop     iy
        inc     sp
        jp      (iy)
    __endasm;
#endif
}
