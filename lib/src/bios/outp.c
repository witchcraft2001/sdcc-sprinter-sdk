#include <sprinter/bios.h>

void outp(u16 port, u8 value) __naked {
    (void)port; (void)value;
    __asm
        ld      c, l
        ld      b, h
        ld      iy, #2
        add     iy, sp
        ld      a, 0 (iy)
        out     (c), a
        pop     iy
        inc     sp
        jp      (iy)
    __endasm;
}
