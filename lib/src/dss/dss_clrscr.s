        .module dss_clrscr
        .globl  _dss_clrscr

        .area   _CODE

_dss_clrscr::
        push    ix
        ld      de,#0x0000
        ld      hl,#0x2050
        ld      bc,#0x0756
        ld      a,#0x20
        rst     #0x10
        ld      de,#0x0000
        ld      c,#0x52
        rst     #0x10
        pop     ix
        ret
