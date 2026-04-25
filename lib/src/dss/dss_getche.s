        .module dss_getche
        .globl  _dss_getche

        .area   _CODE

_dss_getche::
        push    ix
        ld      c, #0x32
        rst     #0x10
        pop     ix
        ld      l, a
        ld      h, #0
        ret
