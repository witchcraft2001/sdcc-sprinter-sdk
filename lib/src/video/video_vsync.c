#include <sprinter/video.h>
#include <sprinter/ports.h>

void video_vsync(void) __naked {
    __asm
_vsync_wait:
        in      a, (#0xC9)
        bit     5, a
        jr      z, _vsync_wait
_vsync_wait2:
        in      a, (#0xC9)
        bit     5, a
        jr      nz, _vsync_wait2
        ret
    __endasm;
}
