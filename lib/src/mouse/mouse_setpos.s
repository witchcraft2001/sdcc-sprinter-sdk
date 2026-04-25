        .module mouse_setpos
        .globl  _mouse_setpos

        .area   _CODE

_mouse_setpos::
        push    ix
        ld      iy, #4
        add     iy, sp
        ld      l, 0 (iy)
        ld      h, 1 (iy)
        ld      e, 2 (iy)
        ld      d, 3 (iy)
        ld      c, #0x04
        rst     #0x30
        pop     ix
        ret
