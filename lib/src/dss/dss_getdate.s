        .module dss_getdate
        .globl  _dss_getdate

        .area   _CODE

_dss_getdate::
        push    ix
        ld      iy, #4
        add     iy, sp
        ld      l, 0 (iy)
        ld      h, 1 (iy)
        push    hl
        ld      c, #0x21
        rst     #0x10
        push    ix
        pop     bc
        pop     hl
        ld      (hl), c
        inc     hl
        ld      (hl), b
        inc     hl
        ld      (hl), d
        inc     hl
        ld      (hl), e
        pop     ix
        ret
