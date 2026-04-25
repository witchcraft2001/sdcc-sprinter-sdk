        .module dss_freemem
        .globl  _dss_freemem

        .area   _CODE

_dss_freemem::
        push    ix
        ld      iy, #4
        add     iy, sp
        ld      a, 0 (iy)
        ld      c, #0x3E
        rst     #0x10
        pop     ix
        ret
