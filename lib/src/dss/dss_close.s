        .module dss_close
        .globl  _dss_close

        .area   _CODE

_dss_close::
        push    ix
        ld      iy, #4
        add     iy, sp
        ld      a, 0 (iy)
        ld      c, #0x12
        rst     #0x10
        pop     ix
        ld      l, a
        ld      h, #0x00
        ret
