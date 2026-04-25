        .module dss_version
        .globl  _dss_version

        .area   _CODE

_dss_version::
        push    ix
        ld      c,#0x00
        rst     #0x10
        pop     ix
        ex      de,hl
        ret
