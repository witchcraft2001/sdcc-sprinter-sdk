        .module dss_setwin
        .globl  _dss_setwin
        .globl  _dss_setwin_page

        .area   _CODE

_dss_setwin::
        push    ix
        ld      iy, #4
        add     iy, sp
        ld      a, 0 (iy)
        add     a, #0x38
        ld      c, a
        ld      a, 1 (iy)
        ld      b, #0
        rst     #0x10
        pop     ix
        ret

_dss_setwin_page::
        push    ix
        ld      iy, #4
        add     iy, sp
        ld      a, 0 (iy)
        add     a, #0x38
        ld      c, a
        ld      a, 1 (iy)
        ld      b, 2 (iy)
        rst     #0x10
        pop     ix
        ret
