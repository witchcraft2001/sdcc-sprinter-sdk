        .module dss_meminfo
        .globl  _dss_meminfo

        .area   _CODE

_dss_meminfo::
        push    ix
        ld      iy,#4
        add     iy,sp
        ld      e,0(iy)
        ld      d,1(iy)
        push    de
        ld      e,2(iy)
        ld      d,3(iy)
        push    de
        ld      c,#0x3C
        rst     #0x10
        push    hl
        pop     de
        pop     hl
        ld      (hl),c
        inc     hl
        ld      (hl),b
        pop     hl
        ld      (hl),e
        inc     hl
        ld      (hl),d
        pop     ix
        ret
