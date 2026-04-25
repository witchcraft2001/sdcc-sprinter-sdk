#include <sprinter/gfx.h>

void gfx_restore_sprite8(u8 screen, u16 x, u8 y) {
    gfx_restore_rect(screen, x, y, 8, 8);
}

void gfx_restore_sprite16(u8 screen, u16 x, u8 y) {
    gfx_restore_rect(screen, x, y, 16, 16);
}

void gfx_restore_sprite24(u8 screen, u16 x, u8 y) {
    gfx_restore_rect(screen, x, y, 24, 24);
}

void gfx_copy_screen(u8 dst_screen, u8 src_screen) {
    gfx_copy_rect(dst_screen, src_screen, 0, 0, 160, 0);
    gfx_copy_rect(dst_screen, src_screen, 160, 0, 160, 0);
}
