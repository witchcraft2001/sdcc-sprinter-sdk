        .module dss_expath
        .globl  _dss_expath

        .area   _CODE

_dss_expath::
        push    ix
        ld      iy,#4
        add     iy,sp
        ld      l,0(iy)
        ld      h,1(iy)
        ld      e,2(iy)
        ld      d,3(iy)
        ld      b,4(iy)
        ld      c,#0x45
        rst     #0x10
        pop     ix
        jr      c,1$
        ld      hl,#0x0000
        ret
1$:
        ld      hl,#0xFFFF
        ret
