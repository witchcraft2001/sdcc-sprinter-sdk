        .module dss_appinfo
        .globl  _dss_appinfo

        .area   _CODE

_dss_appinfo::
        push    ix
        ld      iy,#4
        add     iy,sp
        ld      b,0(iy)
        ld      e,1(iy)
        ld      d,2(iy)
        ld      c,#0x47
        rst     #0x10
        pop     ix
        jr      c,1$
        ld      hl,#0x0000
        ret
1$:
        ld      hl,#0xFFFF
        ret
