#include <sprinter/bios.h>

u8 inp(u16 port) __naked {
    __asm
        ld      c, l
        ld      b, h
        in      a, (c)
        ret
    __endasm;
}
