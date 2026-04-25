#include <sprinter/gfx.h>

static u8 _gfx_screen;

void gfx_set_screen(u8 screen) {
    _gfx_screen = screen & 1;
}

u8 gfx_get_screen(void) {
    return _gfx_screen;
}
