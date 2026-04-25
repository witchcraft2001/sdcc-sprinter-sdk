        .module dss_kbhit
        .globl  _dss_kbhit

        .area   _CODE

_dss_kbhit::
        push    ix
        ld      c, #0x33
        rst     #0x10
        pop     ix
        ld      hl, #0x0000
        or      a
        ret     z
        inc     l
        ret
