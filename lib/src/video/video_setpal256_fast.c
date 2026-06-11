#include <sprinter/video.h>
#include <sprinter/bios.h>

/*
 * video_setpal256_fast - install all 256 palette entries.
 *
 * Goes through bios_setpal() per colour (which is the same proven
 * path that video_setpal_range() uses internally). bios_setpal()
 * builds its 4-byte hardware buffer on the stack, where BIOS A4 can
 * read it across its internal WIN1 remap.
 *
 * This is the "correct, simple, slow" path - 512 BIOS A4 calls
 * (one per colour per page). For a single one-shot palette install
 * at app startup the 20-30 ms cost is invisible. Per-frame fades
 * use the optimised stack-allocated batch path in fade_pal.s instead.
 */
void video_setpal256_fast(const video_rgb8_t *colors) {
    u8 index;
    u8 done;

    index = 0;
    done = 0;
    while (!done) {
        bios_setpal(index, colors->r, colors->g, colors->b);
        colors++;
        index++;
        if (index == 0) done = 1;
    }
}
