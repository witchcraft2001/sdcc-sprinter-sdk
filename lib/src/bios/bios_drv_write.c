#include <sprinter/bios.h>

u8 bios_drv_write(u8 disk, u32 lba, const void *src, u8 count) __naked {
    (void)disk; (void)lba; (void)src; (void)count;
    __asm
        push    ix
        ld      c, a            ; save disk
        ld      iy, #4
        add     iy, sp

        ld      a, 6 (iy)       ; count
        or      a
        jr      z, _bios_drv_write_bad_count
        ld      b, a

        ld      e, 4 (iy)       ; DE = src
        ld      d, 5 (iy)

        ld      l, 0 (iy)       ; IX = LBA bits 15..0
        ld      h, 1 (iy)
        push    hl
        pop     ix
        ld      l, 2 (iy)       ; HL = LBA bits 31..16
        ld      h, 3 (iy)

        ld      a, c            ; A = disk
        ld      c, #0x56        ; DRV_WRITE
        rst     #0x08
        jr      c, _bios_drv_write_done

        xor     a
        jr      _bios_drv_write_done

_bios_drv_write_bad_count:
        ld      a, #1

_bios_drv_write_done:
        pop     ix
        pop     iy
        ld      hl, #7
        add     hl, sp
        ld      sp, hl
        jp      (iy)
    __endasm;
}
