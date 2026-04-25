        .module video_safe_porty
        .globl  _video_safe_porty

        .area   _CODE

_video_safe_porty::
        ld      a,#0xC0
        out     (#0x89),a
        ret
