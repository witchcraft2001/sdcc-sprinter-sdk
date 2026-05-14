        .module keyb_read_row
        .globl  _keyb_read_row

        .area   _CODE

_keyb_read_row::
        push    ix
        ld      ix,#0
        add     ix,sp
        ld      b,4(ix)
        ld      c,#0xFE
        in      a,(c)
        cpl
        and     #0x1F
        ld      l,a
        pop     ix
        ret
