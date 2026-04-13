#include <sprinter/dss.h>

i16 dss_write(u8 fd, const void *buf, u16 count) __naked {
    (void)fd; (void)buf; (void)count;
    __asm
        push    ix
        ex      de, hl          ; HL = buf
        ld      iy, #4
        add     iy, sp
        ld      e, 0 (iy)
        ld      d, 1 (iy)      ; DE = count
        push    de              ; save count (emulator doesn't return DE)
        ld      c, #0x14
        rst     #0x10
        pop     de              ; DE = count (workaround for emulator bug)
        pop     ix
        jr      c, _dss_write_err
        ; return DE = count written
        pop     iy
        inc     sp
        inc     sp
        jp      (iy)
_dss_write_err:
        ld      de, #0xFFFF
        pop     iy
        inc     sp
        inc     sp
        jp      (iy)
    __endasm;
}
