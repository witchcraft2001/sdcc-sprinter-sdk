#include <sprinter/dss.h>

void dss_setwin(u8 win, u8 block) __naked {
    (void)win; (void)block;
    __asm
        push    ix
        add     a, #0x38
        ld      c, a
        ld      a, l
        ld      b, #0
        rst     #0x10
        pop     ix
        ret
    __endasm;
}

void dss_setwin_page(u8 win, u8 block, u8 page_in_block) __naked {
    (void)win; (void)block; (void)page_in_block;
    __asm
        push    ix
        add     a, #0x38
        ld      c, a
        ld      a, l
        ld      iy, #4
        add     iy, sp
        ld      b, 0 (iy)
        rst     #0x10
        pop     ix
        pop     iy
        inc     sp
        jp      (iy)
    __endasm;
}
