#include <sprinter/dss.h>

void dss_meminfo(u16 *total, u16 *free_pages) __naked {
    (void)total; (void)free_pages;
    __asm
        ; sdcccall(1): total -> HL, free_pages -> DE
        push    ix
        push    hl              ; save total ptr
        push    de              ; save free ptr
        ld      c, #0x3C
        rst     #0x10
        ; HL = total pages, BC = free pages
        ex      de, hl          ; DE = total
        pop     hl              ; HL = free_pages ptr
        ld      (hl), c
        inc     hl
        ld      (hl), b
        pop     hl              ; HL = total ptr
        ld      (hl), e
        inc     hl
        ld      (hl), d
        pop     ix
        ret
    __endasm;
}
