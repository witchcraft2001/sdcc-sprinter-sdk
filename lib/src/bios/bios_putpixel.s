        .module bios_putpixel
        .globl  _bios_putpixel

        .area   _CODE

_bios_putpixel::
        push    ix
        ld      iy, #4
        add     iy, sp
        ld      l, 0 (iy)
        ld      h, 1 (iy)
        ld      d, 2 (iy)
        ld      e, 3 (iy)
        in      a, (#0xE2)
        push    af
        ld      a, #0x50
        out     (#0xE2), a
        ld      a, d
        out     (#0x89), a
        ld      bc, #0xC000
        add     hl, bc
        ld      (hl), e
        pop     af
        out     (#0xE2), a
        pop     ix
        ret
