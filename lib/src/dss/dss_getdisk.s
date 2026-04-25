        .module dss_getdisk
        .globl  _dss_getdisk

        .area   _CODE

_dss_getdisk::
        push    ix
        ld      c, #0x02
        rst     #0x10
        pop     ix
        ld      l, a
        ld      h, #0
        ret
