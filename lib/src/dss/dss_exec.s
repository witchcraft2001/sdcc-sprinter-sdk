        .module dss_exec
        .globl  _dss_exec

        .area   _DATA
_dss_exec_saved_ix:
        .ds     2
_dss_exec_saved_iy:
        .ds     2
_dss_exec_saved_ret:
        .ds     2

        .area   _CODE

_dss_exec::
        ld      (_dss_exec_saved_iy), iy
        ld      iy, #2
        add     iy, sp
        ld      e, -2 (iy)
        ld      d, -1 (iy)
        ld      (_dss_exec_saved_ret), de
        ld      l, 0 (iy)
        ld      h, 1 (iy)
        ld      (_dss_exec_saved_ix), ix
        ld      b, #0
        ld      c, #0x40
        rst     #0x10
        ld      ix, (_dss_exec_saved_ix)
        ld      iy, (_dss_exec_saved_iy)
        jr      c, _dss_exec_err_290
        ld      l, a
        ld      h, #0
        ld      de, (_dss_exec_saved_ret)
        push    de
        ret
_dss_exec_err_290:
        ld      hl, #0xFFFF
        ret
