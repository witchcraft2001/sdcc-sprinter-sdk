        .module dss_exit
        .globl  _dss_exit
        .globl  _dss_exit_safe_sp

        .area   _CODE

_dss_exit_safe_sp::
        .dw     0

_dss_exit::
        ld      b, a
        ld      hl, (_dss_exit_safe_sp)
        ld      a, h
        or      l
        jr      z, dss_exit_keep_sp
        ld      sp, hl
dss_exit_keep_sp:
        ld      c, #0x41
        rst     #0x10
        ret
