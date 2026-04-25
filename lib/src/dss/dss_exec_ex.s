        .module dss_exec_ex
        .globl  _dss_exec_ex

        .area   _DATA
_dss_exec_ex_errptr:
        .ds     2
_dss_exec_ex_saved_ix:
        .ds     2
_dss_exec_ex_saved_iy:
        .ds     2
_dss_exec_ex_saved_ret:
        .ds     2

        .area   _CODE

_dss_exec_ex::
        ld      (_dss_exec_ex_saved_iy), iy
        ld      iy, #2
        add     iy, sp
        ld      e, -2 (iy)
        ld      d, -1 (iy)
        ld      (_dss_exec_ex_saved_ret), de
        ld      l, 0 (iy)
        ld      h, 1 (iy)
        ld      e, 2 (iy)
        ld      d, 3 (iy)
        ld      (_dss_exec_ex_errptr), de
        ld      (_dss_exec_ex_saved_ix), ix
        ld      b, #0
        ld      c, #0x40
        rst     #0x10
        ld      ix, (_dss_exec_ex_saved_ix)
        ld      iy, (_dss_exec_ex_saved_iy)
        ld      de, (_dss_exec_ex_errptr)
        jr      c, _dss_exec_ex_err_290
        ld      b, a
        ld      a, d
        or      e
        jr      z, _dss_exec_ex_ok_ret_290
        xor     a
        ld      (de), a
_dss_exec_ex_ok_ret_290:
        ld      l, b
        ld      h, #0
        ld      de, (_dss_exec_ex_saved_ret)
        push    de
        ret
_dss_exec_ex_err_290:
        ld      b, a
        ld      a, d
        or      e
        jr      z, _dss_exec_ex_set_ret_290
        ld      a, b
        ld      (de), a
_dss_exec_ex_set_ret_290:
        ld      hl, #0xFFFF
        ret
