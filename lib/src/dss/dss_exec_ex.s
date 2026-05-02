        .module dss_exec_ex
        .globl  _dss_exec_ex

        .area   _CODE

_dss_exec_ex::
        ; sdcccall(1): path -> HL, err -> DE, return value -> DE.
        pop     bc              ; caller return address
        push    ix
        push    iy
        push    de              ; saved err pointer
        push    bc              ; caller return address
        ld      b, #0
        ld      c, #0x40
        rst     #0x10

        pop     de              ; caller return address
        pop     hl              ; saved err pointer
        pop     iy
        pop     ix
        push    de
        jr      c, _dss_exec_ex_err

        ld      c, a
        ld      a, h
        or      l
        jr      z, _dss_exec_ex_ok_ret
        xor     a
        ld      (hl), a
_dss_exec_ex_ok_ret:
        ld      e, c
        ld      d, #0
        ret

_dss_exec_ex_err:
        ld      c, a
        ld      a, h
        or      l
        jr      z, _dss_exec_ex_set_ret
        ld      a, c
        ld      (hl), a
_dss_exec_ex_set_ret:
        ld      de, #0xFFFF
        ret
