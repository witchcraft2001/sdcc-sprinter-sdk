        .module dss_puts
        .globl  _dss_puts

        .area   _CODE

_dss_puts::
        push    ix
        ld      iy, #4
        add     iy, sp
        ld      l, 0 (iy)
        ld      h, 1 (iy)
        ld      c, #0x5C
        rst     #0x10
        pop     ix
        ret
