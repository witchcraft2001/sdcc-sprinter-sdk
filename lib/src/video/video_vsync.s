        .module video_vsync
        .globl  _video_vsync

        .area   _CODE

_video_vsync::
        ; Sprinter exposes display sync on #FFFE bit 5 only when CBL is active.
        ; Wait for a low->high edge: visible area first, then vertical blank.
        ld      bc,#0x004e
        ld      a,#0x80
        out     (c),a

        ld      de,#0x8000
1$:
        ld      a,#0xff
        in      a,(#0xfe)
        bit     5,a
        jr      z,2$
        dec     de
        ld      a,d
        or      e
        jr      nz,1$
        jr      4$

2$:
        ld      de,#0x8000
3$:
        ld      a,#0xff
        in      a,(#0xfe)
        bit     5,a
        ret     nz
        dec     de
        ld      a,d
        or      e
        jr      nz,3$

4$:
        ; Fallback keeps old behavior instead of hanging if CBL sync is absent.
        ei
        halt
        ret
