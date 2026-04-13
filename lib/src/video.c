/**
 * video.c — Video mode and VRAM wrappers for SDCC sdcccall(1)
 * Save/restore IX around all RST calls.
 */

#include <sprinter/video.h>
#include <sprinter/bios.h>
#include <sprinter/ports.h>

void video_setmode(u8 mode) __naked {
    /* mode in A. DSS.SetVMod: A=mode, B=video_page */
    __asm
        push    ix
        ld      b, #0           ; video page 0
        ld      c, #0x50
        rst     #0x10
        pop     ix
        ret
    __endasm;
}

u8 video_getmode(void) __naked {
    __asm
        push    ix
        ld      c, #0x51
        rst     #0x10
        pop     ix
        ret
    __endasm;
}

void video_swap(void) __naked {
    __asm
        in      a, (#0xC9)
        xor     #0x02
        out     (#0xC9), a
        ret
    __endasm;
}

void video_vsync(void) __naked {
    __asm
_vsync_wait:
        in      a, (#0xC9)
        bit     5, a
        jr      z, _vsync_wait
_vsync_wait2:
        in      a, (#0xC9)
        bit     5, a
        jr      nz, _vsync_wait2
        ret
    __endasm;
}

void video_setpal(u8 index, u8 r, u8 g, u8 b) {
    bios_setpal(index, r, g, b);
}

void video_safe_porty(void) __naked {
    __asm
        ld      a, #0xC0
        out     (#0x89), a
        ret
    __endasm;
}

void video_mapvram(u8 win, u8 page) __naked {
    /* win in A, page in L (2nd u8, 1st in A) */
    __asm
        push    ix
        ld      b, l
        ld      c, #0x38
        rst     #0x10
        pop     ix
        ret
    __endasm;
}
