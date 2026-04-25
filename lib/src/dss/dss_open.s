        .module dss_open
        .globl  _dss_open

        .area   _CODE

_dss_open::
        push    ix
        ld      iy, #4
        add     iy, sp
        ld      l, 0 (iy)
        ld      h, 1 (iy)
        ld      a, 2 (iy)
        inc     a
        cp      #3
        jr      nz, _dss_open_go_290
        xor     a
_dss_open_go_290:
        ld      c, #0x11
        rst     #0x10
        pop     ix
        jr      c, _dss_open_err_290
        ld      l, a
        ld      h, #0
        ret
_dss_open_err_290:
        ld      hl, #0xFFFF
        ret
