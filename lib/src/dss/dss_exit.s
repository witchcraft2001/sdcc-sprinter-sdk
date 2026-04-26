        .module dss_exit
        .globl  _dss_exit

        .area   _CODE

_dss_exit::
        ld      bc, #0x004e
        xor     a
        out     (c), a
        ld      iy, #2
        add     iy, sp
        ld      a, 0 (iy)
        ld      b, 0 (iy)
        ld      c, #0x41
        rst     #0x10
        ret
