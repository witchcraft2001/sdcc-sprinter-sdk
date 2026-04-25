        .module dss_wait
        .globl  _dss_wait

        .area   _CODE

_dss_wait::
        push    ix
        ld      c, #0x42
        rst     #0x10
        pop     ix
        ld      l, a
        ld      h, #0
        ret
