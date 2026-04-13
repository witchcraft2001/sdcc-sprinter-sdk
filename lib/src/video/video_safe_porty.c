#include <sprinter/video.h>
#include <sprinter/ports.h>

void video_safe_porty(void) __naked {
    __asm
        ld      a, #0xC0
        out     (#0x89), a
        ret
    __endasm;
}
