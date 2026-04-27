        .module video_mapvram
        .globl  _video_mapvram

        .area   _CODE

_video_mapvram::
        push    ix
        ld      iy, #4
        add     iy, sp
        ld      a, 0 (iy)
        and     #0x03
        rlca
        rlca
        rlca
        rlca
        rlca
        add     a, #0x82
        ld      c, a
        ld      a, 1 (iy)
        out     (c), a
        pop     ix
        ret
