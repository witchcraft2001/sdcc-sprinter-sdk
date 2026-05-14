        .module joystick
        .globl  _joystick

        .area   _CODE

_joystick::
        ld      b,#0x00
        ld      c,#0x1F
        in      a,(c)
        and     #0x1F
        ld      l,a
        ld      h,#0x00
        ret
