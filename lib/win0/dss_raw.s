; =========================================================================
;  dss_raw.s - raw (no-bounce) DSS wrappers for the win0 layout.
;
;  Same RST #10 wrappers as the standard library, but under *_raw names so the
;  win0 bounce wrappers (win0_dss.c) can call them after marshalling. Linked
;  only in the win0 layout; the normal layout uses the library's dss_* directly
;  with zero overhead. Only functions that hand a pointer to DSS *during* the
;  RST need a raw twin here (functions that fill a struct in the wrapper after
;  the RST are already win0-safe and are not duplicated).
; =========================================================================

        .module dss_raw
        .globl  _dss_puts_raw
        .globl  _dss_open_raw
        .globl  _dss_creat_raw
        .globl  _dss_read_raw
        .globl  _dss_write_raw
        .globl  _dss_curdir_raw
        .globl  _dss_delete_raw
        .globl  _dss_chdir_raw
        .globl  _dss_mkdir_raw
        .globl  _dss_rmdir_raw
        .globl  _dss_rename_raw
        .globl  _dss_ffirst_raw
        .globl  _dss_fnext_raw
        .globl  _dss_getenv_raw
        .globl  _dss_setenv_raw
        .globl  _dss_expath_raw
        .globl  _dss_exec_raw
        .globl  _dss_exec_ex_raw

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

_dss_delete_raw::
        push    ix
        ld      iy, #4
        add     iy, sp
        ld      l, 0 (iy)
        ld      h, 1 (iy)
        ld      c, #0x0E
        rst     #0x10
        pop     ix
        jr      c, 7$
        ld      hl, #0x0000
        ret
7$:
        ld      l, a
        ld      h, #0x00
        ret

_dss_chdir_raw::
        push    ix
        ld      iy, #4
        add     iy, sp
        ld      l, 0 (iy)
        ld      h, 1 (iy)
        ld      c, #0x1D
        rst     #0x10
        pop     ix
        jr      c, 8$
        ld      hl, #0x0000
        ret
8$:
        ld      l, a
        ld      h, #0x00
        ret

_dss_mkdir_raw::
        push    ix
        ld      iy, #4
        add     iy, sp
        ld      l, 0 (iy)
        ld      h, 1 (iy)
        ld      c, #0x1B
        rst     #0x10
        pop     ix
        jr      c, 9$
        ld      hl, #0x0000
        ret
9$:
        ld      l, a
        ld      h, #0x00
        ret

_dss_rmdir_raw::
        push    ix
        ld      iy, #4
        add     iy, sp
        ld      l, 0 (iy)
        ld      h, 1 (iy)
        ld      c, #0x1C
        rst     #0x10
        pop     ix
        jr      c, 10$
        ld      hl, #0x0000
        ret
10$:
        ld      l, a
        ld      h, #0x00
        ret

_dss_rename_raw::
        push    ix
        ld      iy, #4
        add     iy, sp
        ld      l, 0 (iy)
        ld      h, 1 (iy)
        ld      e, 2 (iy)
        ld      d, 3 (iy)
        ld      c, #0x10
        rst     #0x10
        pop     ix
        jr      c, 11$
        ld      hl, #0x0000
        ret
11$:
        ld      l, a
        ld      h, #0x00
        ret

_dss_ffirst_raw::
        push    ix
        ld      iy, #4
        add     iy, sp
        ld      l, 0 (iy)
        ld      h, 1 (iy)
        ld      e, 2 (iy)
        ld      d, 3 (iy)
        ld      a, 4 (iy)
        ld      b, #1
        ld      c, #0x19
        rst     #0x10
        pop     ix
        jr      c, 12$
        ld      hl, #0x0000
        ret
12$:
        ld      hl, #0xFFFF
        ret

_dss_fnext_raw::
        push    ix
        ld      iy, #4
        add     iy, sp
        ld      e, 0 (iy)
        ld      d, 1 (iy)
        ld      c, #0x1A
        rst     #0x10
        pop     ix
        jr      c, 13$
        ld      hl, #0x0000
        ret
13$:
        ld      hl, #0xFFFF
        ret

_dss_getenv_raw::
        push    ix
        ld      iy, #4
        add     iy, sp
        ld      l, 0 (iy)
        ld      h, 1 (iy)
        ld      e, 2 (iy)
        ld      d, 3 (iy)
        ld      b, #1
        ld      c, #0x46
        rst     #0x10
        pop     ix
        jr      c, 14$
        ld      hl, #0x0000
        ret
14$:
        ld      hl, #0xFFFF
        ret

_dss_setenv_raw::
        push    ix
        ld      iy, #4
        add     iy, sp
        ld      l, 0 (iy)
        ld      h, 1 (iy)
        ld      b, #2
        ld      c, #0x46
        rst     #0x10
        pop     ix
        jr      c, 15$
        ld      hl, #0x0000
        ret
15$:
        ld      hl, #0xFFFF
        ret

_dss_expath_raw::
        push    ix
        ld      iy, #4
        add     iy, sp
        ld      l, 0 (iy)
        ld      h, 1 (iy)
        ld      e, 2 (iy)
        ld      d, 3 (iy)
        ld      b, 4 (iy)
        ld      c, #0x45
        rst     #0x10
        pop     ix
        jr      c, 16$
        ld      hl, #0x0000
        ret
16$:
        ld      hl, #0xFFFF
        ret

; EXEC: plain (no SDCC return-fixup). The win0 RST10 trampoline normalises the
; child-exit return, so the standard fixup is unnecessary here. DSS restores the
; parent windows (P1/P2) on child exit; the trampoline restores WIN0=P0.
_dss_exec_raw::
        push    ix
        push    iy
        ld      iy, #6
        add     iy, sp
        ld      l, 0 (iy)
        ld      h, 1 (iy)
        ld      b, #0
        ld      c, #0x40
        rst     #0x10
        pop     iy
        pop     ix
        jr      c, 17$
        ld      l, a
        ld      h, #0
        ret
17$:
        ld      hl, #0xFFFF
        ret

_dss_exec_ex_raw::
        push    ix
        push    iy
        ld      iy, #6
        add     iy, sp
        ld      l, 0 (iy)
        ld      h, 1 (iy)
        ld      b, #0
        ld      c, #0x40
        rst     #0x10
        pop     iy
        pop     ix
        jr      c, 18$
        ld      l, a
        ld      h, #0
        ret
18$:
        ld      l, a            ; error code in A
        ld      h, #0xFF        ; high byte != 0 marks error
        ret
