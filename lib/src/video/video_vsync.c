#include <sprinter/video.h>
#include <sprinter/ports.h>

/*
 * video_vsync - wait for the start of a display frame.
 *
 * Ported from the reference SDK's hardware-sync implementation. Sprinter
 * exposes display sync on #FFFE bit 5 only while CBL is active: #FE.5 is 1
 * while Y > 256 (bottom blank/border) and 0 for Y < 256. We enable CBL, wait
 * for the high->low edge so callers resume at frame start, and TIME OUT to
 * ei/halt instead of hanging if CBL sync is unavailable.
 *
 * (Replaces the earlier port-0xC9 busy-wait, which had no timeout and could
 * hang forever when that sync bit never toggled.)
 */
void video_vsync(void) __naked {
    __asm
        ld      bc, #0x004e
        ld      a, #0x80
        out     (c), a              ; enable CBL sync source

        ld      de, #0x8000
    _vs_hi:
        ld      a, #0xff
        in      a, (#0xfe)
        bit     5, a
        jr      nz, _vs_lo
        dec     de
        ld      a, d
        or      e
        jr      nz, _vs_hi
        jr      _vs_fallback

    _vs_lo:
        ld      de, #0x8000
    _vs_lo2:
        ld      a, #0xff
        in      a, (#0xfe)
        bit     5, a
        ret     z                   ; bit5 went low -> frame start
        dec     de
        ld      a, d
        or      e
        jr      nz, _vs_lo2

    _vs_fallback:
        ei                          ; CBL sync absent: do not hang, pace on INT
        halt
        ret
    __endasm;
}
