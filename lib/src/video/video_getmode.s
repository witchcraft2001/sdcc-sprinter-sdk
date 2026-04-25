        .module video_getmode
        .globl  _video_getmode

        .area   _CODE

_video_getmode::
        push    ix
        ld      c, #0x51
        rst     #0x10
        pop     ix
        ld      l, a
        ld      h, #0
        ret
