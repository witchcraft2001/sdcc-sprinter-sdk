#include <sprinter/video.h>

u8 video_getmode(void) __naked {
    __asm
        push    ix
        ld      c, #0x51
        rst     #0x10
        pop     ix
        ret
    __endasm;
}
