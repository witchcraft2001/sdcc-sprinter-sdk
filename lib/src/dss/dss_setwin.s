        .module dss_setwin
        .globl  _dss_setwin

        .area   _CODE

_dss_setwin::
        push    ix
        ld      iy, #4
        add     iy, sp
        ld      a, 0 (iy)
        ld      b, 1 (iy)
        ld      c, #0x38
        rst     #0x10
        pop     ix
        ret
