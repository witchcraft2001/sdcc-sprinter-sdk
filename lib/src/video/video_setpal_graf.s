        .module video_setpal_graf
        .globl  _video_setpal_graf

        .area   _CODE

_video_setpal_graf::
        push    ix
        ld      b, #1
        ld      e, #0
        xor     a
        ld      c, #0xA6
        rst     #0x08
        pop     ix
        ret
