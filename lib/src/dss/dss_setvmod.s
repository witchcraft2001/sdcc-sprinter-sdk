        .module dss_setvmod
        .globl  _dss_setvmod

        .area   _CODE

_dss_setvmod::
        push    ix
        ld      iy,#4
        add     iy,sp
        ld      a,0(iy)
        ld      b,1(iy)
        ld      c,#0x50
        rst     #0x10
        pop     ix
        jr      c,1$
        ld      hl,#0x0000
        ret
1$:
        ld      l,a
        ld      h,#0x00
        ret
