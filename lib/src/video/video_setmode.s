        .module video_setmode
        .globl  _video_setmode

        .area   _CODE

_video_setmode::
        push    ix
        ld      iy, #4
        add     iy, sp
        ld      a, 0 (iy)
        bit     7, a
        jr      nz, 1$
        ld      bc, #0x004e
        xor     a
        out     (c), a
        ld      a, 0 (iy)
1$:
        ld      b, #0
        ld      c, #0x50
        rst     #0x10
        pop     ix
        ret
