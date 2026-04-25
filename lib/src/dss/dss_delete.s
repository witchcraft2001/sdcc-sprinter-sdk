        .module dss_delete
        .globl  _dss_delete

        .area   _CODE

_dss_delete::
        push    ix
        ld      iy, #4
        add     iy, sp
        ld      l, 0 (iy)
        ld      h, 1 (iy)
        ld      c, #0x0E
        rst     #0x10
        pop     ix
        jr      c, _dss_del_err_290
        ld      hl, #0x0000
        ret
_dss_del_err_290:
        ld      l, a
        ld      h, #0x00
        ret
