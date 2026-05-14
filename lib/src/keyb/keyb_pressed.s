        .module keyb_pressed
        .globl  _keyb_pressed

        .area   _CODE

_keyb_pressed::
        push    ix
        ld      ix,#0
        add     ix,sp

        ld      b,5(ix)         ; high byte: matrix row select
        ld      c,#0xFE
        in      a,(c)
        cpl
        and     #0x1F
        ld      c,a             ; pressed bits, 1 = down

        ld      a,4(ix)         ; low byte: bit index
        and     #0x07
        ld      b,a
        inc     b
        ld      a,#1
bit_loop:
        dec     b
        jr      z,bit_ready
        add     a,a
        jr      bit_loop
bit_ready:
        and     c
        ld      l,#0
        jr      z,done
        inc     l
done:
        pop     ix
        ret
