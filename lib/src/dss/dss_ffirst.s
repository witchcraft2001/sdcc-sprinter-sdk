        .module dss_ffirst
        .globl  _dss_ffirst

        .area   _CODE

_dss_ffirst::
        push    ix
        ld      iy, #4
        add     iy, sp
        ld      l, 0 (iy)
        ld      h, 1 (iy)
        ld      e, 2 (iy)
        ld      d, 3 (iy)
        ld      a, 4 (iy)
        ld      b, #1
        ld      c, #0x19
        rst     #0x10
        pop     ix
        jr      c, _dss_ff_err_290
        ld      hl, #0x0000
        ret
_dss_ff_err_290:
        ld      hl, #0xFFFF
        ret
