        .module dss_fnext
        .globl  _dss_fnext

        .area   _CODE

_dss_fnext::
        push    ix
        ld      iy, #4
        add     iy, sp
        ld      e, 0 (iy)
        ld      d, 1 (iy)
        ld      c, #0x1A
        rst     #0x10
        pop     ix
        jr      c, _dss_fn_err_290
        ld      hl, #0x0000
        ret
_dss_fn_err_290:
        ld      hl, #0xFFFF
        ret
