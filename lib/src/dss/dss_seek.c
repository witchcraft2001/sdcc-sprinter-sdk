#include <sprinter/dss.h>

i16 dss_seek(u8 fd, u32 offset, u8 origin) __naked {
    (void)fd; (void)offset; (void)origin;
    __asm
        push    ix
        ld      iy, #4
        add     iy, sp
        ld      l, 0 (iy)
        ld      h, 1 (iy)      ; HL = offset low word
        push    hl
        pop     ix              ; IX = offset low word
        ld      l, 2 (iy)
        ld      h, 3 (iy)      ; HL = offset high word
        ld      b, 4 (iy)      ; origin
        ld      c, #0x15
        rst     #0x10
        pop     ix
        jr      c, _dss_seek_err
        ld      de, #0
        pop     iy
        ld      hl, #5
        add     hl, sp
        ld      sp, hl
        jp      (iy)
_dss_seek_err:
        ld      de, #0xFFFF
        pop     iy
        ld      hl, #5
        add     hl, sp
        ld      sp, hl
        jp      (iy)
    __endasm;
}
