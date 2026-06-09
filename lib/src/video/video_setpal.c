#include <sprinter/video.h>
#include <sprinter/bios.h>

void video_setpal(u8 index, u8 r, u8 g, u8 b) {
    bios_setpal(index, r, g, b);
}
