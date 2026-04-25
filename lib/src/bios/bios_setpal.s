        .module bios_setpal
        .globl  _bios_setpal

        .area   _CODE

_bios_setpal::
        push    ix
        ld      iy,#4
        add     iy,sp
        ld      d,0(iy)
        ld      e,1(iy)

        ld      a,#0xC0
        out     (#0x89),a
        ld      a,d
        out     (#0x89),a
        ld      a,e
        out     (#0x89),a
        ld      a,2(iy)
        out     (#0x89),a
        ld      a,3(iy)
        out     (#0x89),a

        pop     ix
        ret
