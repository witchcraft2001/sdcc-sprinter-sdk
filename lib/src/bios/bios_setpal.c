#include <sprinter/bios.h>
#include <sprinter/ports.h>

void bios_setpal(u8 index, u8 r, u8 g, u8 b) __naked {
    /* index=A, r=L, g+b on stack (2 bytes). Callee cleans 2. */
    (void)index; (void)r; (void)g; (void)b;
    __asm
        push    ix
        ld      d, a            ; D = index
        ld      e, l            ; E = r
        ld      iy, #4
        add     iy, sp

        ld      a, #0xC0
        out     (#0x89), a      ; palette command
        ld      a, d
        out     (#0x89), a      ; index
        ld      a, e
        out     (#0x89), a      ; R
        ld      a, 0 (iy)
        out     (#0x89), a      ; G
        ld      a, 1 (iy)
        out     (#0x89), a      ; B

        pop     ix
        pop     iy
        inc     sp
        inc     sp
        jp      (iy)
    __endasm;
}
