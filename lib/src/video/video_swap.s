        .module video_swap
        .globl  _video_swap

        .area   _CODE

_video_swap::
        in      a,(#0xC9)
        xor     #0x02
        out     (#0xC9),a
        ret
