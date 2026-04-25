        .module outp
        .globl  _outp

        .area   _CODE

_outp::
        push    ix
        ld      iy,#4
        add     iy,sp
        ld      c,0(iy)
        ld      b,1(iy)
        ld      a,2(iy)
        out     (c),a
        pop     ix
        ret
