        .module dss_curdir
        .globl  _dss_curdir

        .area   _CODE

_dss_curdir::
        push    ix
        ld      iy, #4
        add     iy, sp
        ld      l, 0 (iy)
        ld      h, 1 (iy)
        ld      c, #0x1E
        rst     #0x10
        pop     ix
        jr      c, _dss_curdir_err_290
        ld      hl, #0x0000
        ret
_dss_curdir_err_290:
        ld      l, a
        ld      h, #0x00
        ret
