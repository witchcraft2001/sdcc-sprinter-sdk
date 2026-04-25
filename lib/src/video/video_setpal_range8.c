#include <sprinter/video.h>
#include <sprinter/bios.h>

void video_setpal_range8(u8 first, u16 count, const video_rgb8_t *colors) {
    while (count--) {
        bios_setpal(first, colors->r >> 2, colors->g >> 2, colors->b >> 2);
        first++;
        colors++;
    }
}
