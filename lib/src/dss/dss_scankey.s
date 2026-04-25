        .module dss_scankey
        .globl  _dss_scankey

        .area   _CODE

_dss_scankey::
        push    ix
        ld      iy, #4
        add     iy, sp
        ld      l, 0 (iy)
        ld      h, 1 (iy)
        push    hl
        ld      c, #0x31
        rst     #0x10
        jr      z, no_key_290
        pop     hl
        ld      (hl), a
        inc     hl
        ld      (hl), d
        inc     hl
        ld      (hl), b
        inc     hl
        ld      (hl), c
        pop     ix
        ld      hl, #0x0001
        ret
no_key_290:
        pop     hl
        pop     ix
        ld      hl, #0x0000
        ret
