#include <sprinter/video.h>

void video_mapvram(u8 win, u8 page) __naked {
    /* win in A, page in L (2nd u8, 1st in A) */
    (void)win; (void)page;
    __asm
        push    ix
        add     a, #0x38
        ld      c, a
        ld      a, l
        ld      b, #0
        rst     #0x10
        pop     ix
        ret
    __endasm;
}
