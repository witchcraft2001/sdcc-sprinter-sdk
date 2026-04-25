        .module video_mapvram
        .globl  _video_mapvram

        .area   _CODE

_video_mapvram::
        push    ix
        ld      iy, #4
        add     iy, sp
        ld      a, 0 (iy)
        ld      b, 1 (iy)
        ld      c, #0x38
        rst     #0x10
        pop     ix
        ret
