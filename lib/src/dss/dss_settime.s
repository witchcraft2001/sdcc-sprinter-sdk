        .module dss_settime
        .globl  _dss_settime

        .area   _CODE

_dss_settime::
        push    ix
        ld      iy,#4
        add     iy,sp
        ld      l,0(iy)
        ld      h,1(iy)
        ld      e,2(iy)
        ld      d,3(iy)
        push    de
        ld      e,(hl)
        inc     hl
        ld      d,(hl)
        inc     hl
        push    de
        pop     ix
        ld      d,(hl)
        inc     hl
        ld      e,(hl)
        pop     hl
        ld      a,(hl)
        inc     hl
        push    af
        ld      a,(hl)
        inc     hl
        inc     hl
        ld      b,(hl)
        ld      h,a
        pop     af
        ld      l,a
        ld      c,#0x22
        rst     #0x10
        pop     ix
        ret
