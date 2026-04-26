#include <sprinter/gfx.h>

void gfx_scroll_rect(u8 screen, u16 dst_x, u8 dst_y,
                     u16 src_x, u8 src_y, u8 width, u8 height) {
    gfx_blit_rect(screen, dst_x, dst_y, screen, src_x, src_y, width, height);
}
