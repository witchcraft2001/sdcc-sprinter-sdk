; =========================================================================
;  dss_raw.s - raw (no-bounce) DSS wrappers for the win0 layout.
;
;  Same RST #10 wrappers as the standard library, but under *_raw names so the
;  win0 bounce wrappers (win0_dss.c) can call them after marshalling. Linked
;  only in the win0 layout; the normal layout uses the library's dss_* directly
;  with zero overhead.
; =========================================================================

        .module dss_raw
        .globl  _dss_puts_raw
        .globl  _dss_open_raw
        .globl  _dss_creat_raw
        .globl  _dss_read_raw
        .globl  _dss_write_raw
        .globl  _dss_curdir_raw

        .area   _CODE

_dss_puts_raw::
        push    ix
        ld      iy, #4
        add     iy, sp
        ld      l, 0 (iy)
        ld      h, 1 (iy)
        ld      c, #0x5C
        rst     #0x10
        pop     ix
        ret

_dss_open_raw::
        push    ix
        ld      iy, #4
        add     iy, sp
        ld      l, 0 (iy)
        ld      h, 1 (iy)
        ld      a, 2 (iy)
        inc     a
        cp      #3
        jr      nz, 1$
        xor     a
1$:
        ld      c, #0x11
        rst     #0x10
        pop     ix
        jr      c, 2$
        ld      l, a
        ld      h, #0
        ret
2$:
        ld      hl, #0xFFFF
        ret

_dss_creat_raw::
        push    ix
        ld      iy, #4
        add     iy, sp
        ld      l, 0 (iy)
        ld      h, 1 (iy)
        ld      a, #0x20
        ld      c, #0x0A
        rst     #0x10
        pop     ix
        jr      c, 3$
        ld      l, a
        ld      h, #0
        ret
3$:
        ld      hl, #0xFFFF
        ret

_dss_read_raw::
        push    ix
        ld      iy, #4
        add     iy, sp
        ld      a, 0 (iy)
        ld      l, 1 (iy)
        ld      h, 2 (iy)
        ld      e, 3 (iy)
        ld      d, 4 (iy)
        ld      c, #0x13
        rst     #0x10
        pop     ix
        jr      c, 4$
        ex      de, hl
        ret
4$:
        ld      hl, #0xFFFF
        ret

_dss_write_raw::
        push    ix
        ld      iy, #4
        add     iy, sp
        ld      a, 0 (iy)
        ld      l, 1 (iy)
        ld      h, 2 (iy)
        ld      e, 3 (iy)
        ld      d, 4 (iy)
        push    de
        ld      c, #0x14
        rst     #0x10
        pop     de
        pop     ix
        jr      c, 5$
        ex      de, hl
        ret
5$:
        ld      hl, #0xFFFF
        ret

_dss_curdir_raw::
        push    ix
        ld      iy, #4
        add     iy, sp
        ld      l, 0 (iy)
        ld      h, 1 (iy)
        ld      c, #0x1E
        or      a
        rst     #0x10
        pop     ix
        jr      c, 6$
        ld      hl, #0x0000
        ret
6$:
        ld      l, a
        ld      h, #0x00
        ret
