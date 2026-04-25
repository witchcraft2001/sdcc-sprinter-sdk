        .module dss_rmdir
        .globl  _dss_rmdir

        .area   _CODE

_dss_rmdir::
        push    ix
        ld      iy, #4
        add     iy, sp
        ld      l, 0 (iy)
        ld      h, 1 (iy)
        ld      c, #0x1C
        rst     #0x10
        pop     ix
        jr      c, _dss_rmdir_err_290
        ld      hl, #0x0000
        ret
_dss_rmdir_err_290:
        ld      l, a
        ld      h, #0x00
        ret
