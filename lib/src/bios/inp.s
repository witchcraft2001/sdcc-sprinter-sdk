        .module inp
        .globl  _inp

        .area   _CODE

_inp::
        push    ix
        ld      iy,#4
        add     iy,sp
        ld      c,0(iy)
        ld      b,1(iy)
        in      a,(c)
        pop     ix
        ld      l,a
        ld      h,#0x00
        ret
