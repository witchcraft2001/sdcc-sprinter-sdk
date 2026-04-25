        .module dss_read
        .globl  _dss_read

        .area   _CODE

_dss_read::
        push    ix
        ld      iy, #4
        add     iy, sp
        ld      a, 0 (iy)
        ld      l, 1 (iy)
        ld      h, 2 (iy)
        ld      e, 3 (iy)
        ld      d, 4 (iy)
        ld      c, #0x13
        rst     #0x10
        pop     ix
        jr      c, _dss_read_err_290
        ex      de, hl
        ret
_dss_read_err_290:
        ld      hl, #0xFFFF
        ret
