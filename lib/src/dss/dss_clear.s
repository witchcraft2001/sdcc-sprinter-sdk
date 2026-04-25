        .module dss_clear
        .globl  _dss_clear

        .area   _CODE

_dss_clear::
        push    ix
        ld      iy,#4
        add     iy,sp
        ld      e,0(iy)
        ld      d,1(iy)
        ld      l,2(iy)
        ld      h,3(iy)
        ld      b,4(iy)
        ld      a,5(iy)
        ld      c,#0x56
        rst     #0x10
        pop     ix
        ret
