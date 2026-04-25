        .module dss_getenv
        .globl  _dss_getenv

        .area   _CODE

_dss_getenv::
        push    ix
        ld      iy,#4
        add     iy,sp
        ld      l,0(iy)
        ld      h,1(iy)
        ld      e,2(iy)
        ld      d,3(iy)
        ld      b,#1
        ld      c,#0x46
        rst     #0x10
        pop     ix
        jr      c,1$
        ld      hl,#0x0000
        ret
1$:
        ld      hl,#0xFFFF
        ret
