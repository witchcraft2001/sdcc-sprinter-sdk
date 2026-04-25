        .module dss_getmem
        .globl  _dss_getmem

        .area   _CODE

_dss_getmem::
        push    ix
        ld      b,#1
        ld      c,#0x3D
        rst     #0x10
        pop     ix
        ld      l,a
        ld      h,#0x00
        ret
