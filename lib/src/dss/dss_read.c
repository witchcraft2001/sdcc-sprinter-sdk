#include <sprinter/dss.h>

i16 dss_read(u8 fd, void *buf, u16 count) __naked {
    (void)fd; (void)buf; (void)count;
    __asm
        push    ix
        ex      de, hl          ; HL = buf
        ld      iy, #4
        add     iy, sp
        ld      e, 0 (iy)
        ld      d, 1 (iy)      ; DE = count
        ld      c, #0x13
        rst     #0x10
        pop     ix
        jr      c, _dss_read_err
        ; DE = bytes read (set by DSS)
        pop     iy              ; return address
        inc     sp
        inc     sp              ; clean 2 bytes
        jp      (iy)
_dss_read_err:
        ld      de, #0xFFFF
        pop     iy
        inc     sp
        inc     sp
        jp      (iy)
    __endasm;
}
