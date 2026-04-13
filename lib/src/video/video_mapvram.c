#include <sprinter/video.h>

void video_mapvram(u8 win, u8 page) __naked {
    /* win in A, page in L (2nd u8, 1st in A) */
    __asm
        push    ix
        ld      b, l
        ld      c, #0x38
        rst     #0x10
        pop     ix
        ret
    __endasm;
}
