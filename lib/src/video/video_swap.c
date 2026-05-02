#include <sprinter/video.h>
#include <sprinter/ports.h>

void video_swap(void) __naked {
    __asm
        in      a, (#0xC9)
        xor     #0x01
        out     (#0xC9), a
        ret
    __endasm;
}
