        .module dss_setdisk
        .globl  _dss_setdisk

        .area   _CODE

_dss_setdisk::
        push    ix
        ld      iy, #4
        add     iy, sp
        ld      a, 0 (iy)
        ld      c, #0x01
        rst     #0x10
        pop     ix
        ret
