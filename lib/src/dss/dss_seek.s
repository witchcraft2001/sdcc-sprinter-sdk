        .module dss_seek
        .globl  _dss_seek

        .area   _CODE

_dss_seek::
        push    ix
        ld      iy,#4
        add     iy,sp
        ld      a,0(iy)
        ld      l,1(iy)
        ld      h,2(iy)
        push    hl
        pop     ix
        ld      l,3(iy)
        ld      h,4(iy)
        ld      b,5(iy)
        ld      c,#0x15
        rst     #0x10
        pop     ix
        jr      c,1$
        ld      hl,#0x0000
        ret
1$:
        ld      hl,#0xFFFF
        ret
