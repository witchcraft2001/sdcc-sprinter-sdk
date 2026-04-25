        .module dss_setenv
        .globl  _dss_setenv

        .area   _CODE

_dss_setenv::
        push    ix
        ld      iy, #4
        add     iy, sp
        ld      l, 0 (iy)
        ld      h, 1 (iy)
        ld      b, #2
        ld      c, #0x46
        rst     #0x10
        pop     ix
        jr      c, _dss_se_err_290
        ld      hl, #0x0000
        ret
_dss_se_err_290:
        ld      hl, #0xFFFF
        ret
