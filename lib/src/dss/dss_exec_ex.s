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
        ; sdcccall(1): path -> HL, err -> DE, return value -> DE.
        ; DSS.EXEC resumes a successful parent via the stack return address,
        ; so install a continuation and keep wrapper state outside the stack.
        ld      (_dss_exec_ex_errptr), de
        ld      (_dss_exec_ex_saved_ix), ix
        ld      (_dss_exec_ex_saved_iy), iy
        ld      iy, #0
        add     iy, sp
        ld      e, 0 (iy)
        ld      d, 1 (iy)
        ld      (_dss_exec_ex_saved_ret), de
        ld      de, #_dss_exec_ex_done
        ld      0 (iy), e
        ld      1 (iy), d
        ld      b, #0
        ld      c, #0x40
        rst     #0x10

        ; Error path: RST returned directly, so restore caller return address.
        ld      hl, (_dss_exec_ex_saved_ret)
        ex      (sp), hl
        jr      _dss_exec_ex_return

_dss_exec_ex_done:
        ; Success path: DSS returned to the continuation address.
        ld      de, (_dss_exec_ex_saved_ret)
        push    de

_dss_exec_ex_return:
        ld      ix, (_dss_exec_ex_saved_ix)
        ld      iy, (_dss_exec_ex_saved_iy)
        ld      de, (_dss_exec_ex_errptr)
        jr      c, _dss_exec_ex_err

        ld      b, a
        ld      a, d
        or      e
        jr      z, _dss_exec_ex_ok_ret
        xor     a
        ld      (de), a
_dss_exec_ex_ok_ret:
        ld      e, b
        ld      d, #0
        ret

_dss_exec_ex_err:
        ld      b, a
        ld      a, d
        or      e
        jr      z, _dss_exec_ex_set_ret
        ld      a, b
        ld      (de), a
_dss_exec_ex_set_ret:
        ld      de, #0xFFFF
        ret
