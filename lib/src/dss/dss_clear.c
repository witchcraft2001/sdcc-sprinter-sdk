#include <sprinter/dss.h>

void dss_clear(u8 x, u8 y, u8 w, u8 h, u8 color, u8 attr) __naked {
    (void)x; (void)y; (void)w; (void)h; (void)color; (void)attr;
    __asm
        ; sdcccall(1): x -> A, y -> L, rest on stack
        push    ix
        ld      d, l            ; D = y
        ld      e, a            ; E = x
        ld      iy, #4
        add     iy, sp
        ld      l, 0 (iy)      ; L = width
        ld      h, 1 (iy)      ; H = height
        ld      b, 2 (iy)      ; B = color
        ld      a, 3 (iy)      ; A = attr
        ld      c, #0x56
        rst     #0x10
        pop     ix
        ; callee cleans 4 stack bytes
        pop     iy
        ld      hl, #4
        add     hl, sp
        ld      sp, hl
        jp      (iy)
    __endasm;
}
