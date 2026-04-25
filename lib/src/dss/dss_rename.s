        .module dss_rename
        .globl  _dss_rename

        .area   _CODE

_dss_rename::
        push    ix
        ld      iy,#4
        add     iy,sp
        ld      l,0(iy)
        ld      h,1(iy)
        ld      e,2(iy)
        ld      d,3(iy)
        ld      c,#0x10
        rst     #0x10
        pop     ix
        jr      c,1$
        ld      hl,#0x0000
        ret
1$:
        ld      l,a
        ld      h,#0x00
        ret
