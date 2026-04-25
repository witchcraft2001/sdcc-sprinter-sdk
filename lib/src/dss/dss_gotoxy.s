        .module dss_gotoxy
        .globl  _dss_gotoxy

        .area   _CODE

_dss_gotoxy::
        push    ix
        ld      iy, #4
        add     iy, sp
        ld      e, 0 (iy)
        ld      d, 1 (iy)
        dec     d
        dec     e
        ld      c, #0x52
        rst     #0x10
        pop     ix
        ret
