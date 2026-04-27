        .module dss_gettime
        .globl  _dss_gettime

        .area   _CODE

_dss_gettime::
        push    ix
        ld      iy, #4
        add     iy, sp
        ld      l, 0 (iy)
        ld      h, 1 (iy)
        push    hl
        ld      c, #0x21
        rst     #0x10
        ex      (sp), hl
        pop     de
        ld      (hl), e
        inc     hl
        ld      (hl), d
        inc     hl
        ld      (hl), #0
        inc     hl
        ld      (hl), b
        pop     ix
        ret
