/**
 * video.c — Video mode and VRAM wrappers for SDCC
 */

#include <sprinter/video.h>
#include <sprinter/bios.h>
#include <sprinter/ports.h>

void video_setmode(u8 mode) __z88dk_fastcall __naked {
    __asm
        ld      a, l            ; mode
        ld      c, #0x50        ; DSS.SetVMod
        rst     #0x10
        ret
    __endasm;
}

u8 video_getmode(void) __naked {
    __asm
        ld      c, #0x51        ; DSS.GetVMod
        rst     #0x10
        ld      l, a
        ret
    __endasm;
}

void video_swap(void) __naked {
    __asm
        in      a, (#0xC9)      ; read RGMOD
        xor     #0x02           ; toggle display buffer bit
        out     (#0xC9), a
        ret
    __endasm;
}

void video_vsync(void) __naked {
    __asm
_vsync_wait:
        in      a, (#0xC9)      ; read RGMOD
        bit     5, a            ; check VBlank bit
        jr      z, _vsync_wait  ; wait until VBlank
_vsync_wait2:
        in      a, (#0xC9)
        bit     5, a
        jr      nz, _vsync_wait2 ; wait until VBlank ends
        ret
    __endasm;
}

void video_setpal(u8 index, u8 r, u8 g, u8 b) {
    bios_setpal(index, r, g, b);
}

void video_mapvram(u8 win, u8 page) __naked {
    __asm
        ld      hl, #2
        add     hl, sp
        ld      a, (hl)         ; win
        inc     hl
        inc     hl
        ld      b, (hl)         ; page (VRAM page number)
        ld      c, #0x38        ; DSS.SetWin
        rst     #0x10
        ret
    __endasm;
}
