        .module dss_waitkey
        .globl  _dss_waitkey

        .area   _CODE

_dss_waitkey::
        push    ix
        ld      c, #0x30
        rst     #0x10
        pop     ix
        ld      l, a
        ld      h, #0
        ret
