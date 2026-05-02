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
        ; sdcccall(1): path -> HL, return value -> DE.
        ; DSS.EXEC resumes a successful parent via the stack return address,
        ; so install a continuation and keep wrapper state outside the stack.
        ld      (_dss_exec_saved_ix), ix
        ld      (_dss_exec_saved_iy), iy
        ld      iy, #0
        add     iy, sp
        ld      e, 0 (iy)
        ld      d, 1 (iy)
        ld      (_dss_exec_saved_ret), de
        ld      de, #_dss_exec_done
        ld      0 (iy), e
        ld      1 (iy), d
        ld      b, #0
        ld      c, #0x40
        rst     #0x10

        ; Error path: RST returned directly, so restore caller return address.
        ld      hl, (_dss_exec_saved_ret)
        ex      (sp), hl
        jr      _dss_exec_return

_dss_exec_done:
        ; Success path: DSS returned to the continuation address.
        ld      de, (_dss_exec_saved_ret)
        push    de

_dss_exec_return:
        ld      ix, (_dss_exec_saved_ix)
        ld      iy, (_dss_exec_saved_iy)
        jr      c, _dss_exec_err
        ld      e, a
        ld      d, #0
        ret

_dss_exec_err:
        ld      de, #0xFFFF
        ret
