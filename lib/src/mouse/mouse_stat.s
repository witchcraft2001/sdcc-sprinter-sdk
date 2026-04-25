        .module mouse_stat
        .globl  _mouse_stat

        .area   _CODE

_mouse_stat::
        push    ix
        ld      iy,#4
        add     iy,sp
        ld      l,0(iy)
        ld      h,1(iy)
        push    hl
        ld      c,#0x03
        rst     #0x30
        ex      (sp),hl
        ld      (hl),a
        inc     hl
        pop     bc
        ld      (hl),c
        inc     hl
        ld      (hl),b
        inc     hl
        ld      (hl),e
        inc     hl
        ld      (hl),d
        pop     ix
        ret
