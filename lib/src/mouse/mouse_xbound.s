        .module mouse_xbound
        .globl  _mouse_xbound

        .area   _CODE

_mouse_xbound::
        push    ix
        ld      iy, #4
        add     iy, sp
        ld      l, 0 (iy)
        ld      h, 1 (iy)
        ld      e, 2 (iy)
        ld      d, 3 (iy)
        ld      c, #0x05
        rst     #0x30
        pop     ix
        ret
