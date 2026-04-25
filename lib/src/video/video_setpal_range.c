#include <sprinter/video.h>
#include <sprinter/bios.h>

void video_setpal_range(u8 first, u16 count, const video_rgb6_t *colors) {
    while (count--) {
        bios_setpal(first, colors->r, colors->g, colors->b);
        first++;
        colors++;
    }
}
