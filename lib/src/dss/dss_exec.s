        .module dss_exec
        .globl  _dss_exec

        .area   _CODE

_dss_exec::
        ; sdcccall(1): path -> HL, return value -> DE.
        pop     de              ; caller return address
        push    ix
        push    iy
        push    de
        ld      b, #0
        ld      c, #0x40
        rst     #0x10

        pop     de              ; caller return address
        pop     iy
        pop     ix
        push    de
        jr      c, _dss_exec_err
        ld      c, a
        ld      e, c
        ld      d, #0
        ret

_dss_exec_err:
        ld      de, #0xFFFF
        ret
