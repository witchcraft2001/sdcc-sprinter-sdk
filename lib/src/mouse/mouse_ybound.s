        .module mouse_ybound
        .globl  _mouse_ybound

        .area   _CODE

_mouse_ybound::
        push    ix
        ld      iy, #4
        add     iy, sp
        ld      l, 0 (iy)
        ld      h, 1 (iy)
        ld      e, 2 (iy)
        ld      d, 3 (iy)
        ld      c, #0x06
        rst     #0x30
        pop     ix
        ret
