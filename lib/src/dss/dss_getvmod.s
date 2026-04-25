        .module dss_getvmod
        .globl  _dss_getvmod

        .area   _CODE

_dss_getvmod::
        push    ix
        ld      iy,#4
        add     iy,sp
        ld      e,0(iy)
        ld      d,1(iy)
        push    de
        ld      e,2(iy)
        ld      d,3(iy)
        push    de
        ld      c,#0x51
        rst     #0x10
        pop     hl
        ld      (hl),b
        pop     hl
        ld      (hl),a
        pop     ix
        ret
