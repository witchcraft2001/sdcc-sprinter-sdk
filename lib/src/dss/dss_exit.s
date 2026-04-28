        .module dss_exit
        .globl  _dss_exit
        .globl  _dss_exit_safe_sp

        .area   _CODE

_dss_exit_safe_sp::
        .dw     0

_dss_exit::
        ld      bc, #0x004e
        xor     a
        out     (c), a
        ld      iy, #2
        add     iy, sp
        ld      b, 0 (iy)
        ld      hl, (_dss_exit_safe_sp)
        ld      a, h
        or      l
        jr      z, _dss_exit_keep_sp
        ld      sp, hl
_dss_exit_keep_sp:
        ld      c, #0x41
        rst     #0x10
        ret
