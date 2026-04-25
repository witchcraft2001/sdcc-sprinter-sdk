        .module dss_write
        .globl  _dss_write

        .area   _CODE

_dss_write::
        push    ix
        ld      iy, #4
        add     iy, sp
        ld      a, 0 (iy)
        ld      l, 1 (iy)
        ld      h, 2 (iy)
        ld      e, 3 (iy)
        ld      d, 4 (iy)
        push    de
        ld      c, #0x14
        rst     #0x10
        pop     de
        pop     ix
        jr      c, _dss_write_err_290
        ex      de, hl
        ret
_dss_write_err_290:
        ld      hl, #0xFFFF
        ret
