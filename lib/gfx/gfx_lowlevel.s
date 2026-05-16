        .module gfx_lowlevel
        .globl  _gfx_get_active_screen
        .globl  _gfx_flip
        .globl  _gfx_draw_sprite8
        .globl  _gfx_draw_sprite16
        .globl  _gfx_draw_sprite24
        .globl  _gfx_draw_sprite8_win0
        .globl  _gfx_draw_sprite16_win0
        .globl  _gfx_draw_sprite24_win0
        .globl  _gfx_blit_line_accel
        .globl  _gfx_restore_rect
        .globl  _gfx_copy_rect

        .area   _CODE

; A = GFX flags, returns A = VRAM page selector for WIN3.
_gfx_page_from_flags:
        ld      b, a
        ld      a, #0x50
        bit     0, b
        jr      z, 1$
        add     a, #0x08
1$:
        bit     1, b
        jr      z, 2$
        add     a, #0x04
2$:
        ret

; A = screen, HL = x. Returns HL = 0xC000 + x + screen * 320.
_gfx_dest_addr:
        push    de
        ld      de, #0xC000
        add     hl, de
        and     #1
        jr      z, 3$
        ld      de, #0x0140
        add     hl, de
3$:
        pop     de
        ret

_gfx_get_active_screen::
        in      a, (#0xC9)
        and     #1
        ld      l, a
        ld      h, #0
        ret

_gfx_flip::
        in      a, (#0xC9)
        xor     #1
        out     (#0xC9), a
        ret

_gfx_draw_sprite8::
        push    ix
        ld      iy, #4
        add     iy, sp
        ld      l, 1 (iy)
        ld      h, 2 (iy)
        ld      a, 0 (iy)
        call    _gfx_dest_addr
        ex      de, hl
        ld      l, 4 (iy)
        ld      h, 5 (iy)
        ld      c, 3 (iy)
        in      a, (#0xE2)
        push    af
        ld      a, 6 (iy)
        call    _gfx_page_from_flags
        out     (#0xE2), a
        ld      b, #8
4$:
        push    bc
        ld      a, c
        out     (#0x89), a
        push    de
        ld      bc, #8
        ldir
        pop     de
        pop     bc
        inc     c
        djnz    4$
        pop     af
        out     (#0xE2), a
        ld      a, #0xC0
        out     (#0x89), a
        pop     ix
        ret

_gfx_draw_sprite16::
        push    ix
        ld      iy, #4
        add     iy, sp
        ld      l, 1 (iy)
        ld      h, 2 (iy)
        ld      a, 0 (iy)
        call    _gfx_dest_addr
        ex      de, hl
        ld      l, 4 (iy)
        ld      h, 5 (iy)
        ld      c, 3 (iy)
        in      a, (#0xE2)
        push    af
        ld      a, 6 (iy)
        call    _gfx_page_from_flags
        out     (#0xE2), a
        ld      b, #16
5$:
        push    bc
        ld      a, c
        out     (#0x89), a
        di
        ld      d, d
        ld      a, #16
        ld      l, l
        ld      a, (hl)
        ld      (de), a
        ld      b, b
        ei
        ld      bc, #16
        add     hl, bc
        pop     bc
        inc     c
        djnz    5$
        pop     af
        out     (#0xE2), a
        ld      a, #0xC0
        out     (#0x89), a
        pop     ix
        ret

_gfx_draw_sprite24::
        push    ix
        ld      iy, #4
        add     iy, sp
        ld      l, 1 (iy)
        ld      h, 2 (iy)
        ld      a, 0 (iy)
        call    _gfx_dest_addr
        ex      de, hl
        ld      l, 4 (iy)
        ld      h, 5 (iy)
        ld      c, 3 (iy)
        in      a, (#0xE2)
        push    af
        ld      a, 6 (iy)
        call    _gfx_page_from_flags
        out     (#0xE2), a
        ld      b, #24
6$:
        push    bc
        ld      a, c
        out     (#0x89), a
        di
        ld      d, d
        ld      a, #24
        ld      l, l
        ld      a, (hl)
        ld      (de), a
        ld      b, b
        ei
        ld      bc, #24
        add     hl, bc
        pop     bc
        inc     c
        djnz    6$
        pop     af
        out     (#0xE2), a
        ld      a, #0xC0
        out     (#0x89), a
        pop     ix
        ret

_gfx_draw_sprite8_win0::
        push    ix
        ld      iy, #4
        add     iy, sp
        ld      l, 1 (iy)
        ld      h, 2 (iy)
        ld      a, 0 (iy)
        call    _gfx_dest_addr
        ex      de, hl
        ld      l, 4 (iy)
        ld      h, 5 (iy)
        ld      c, 3 (iy)
        in      a, (#0xE2)
        push    af
        ld      a, 6 (iy)
        call    _gfx_page_from_flags
        out     (#0xE2), a
        ld      b, #8
draw8_win0_loop:
        push    bc
        ld      a, c
        out     (#0x89), a
        push    de
        ld      bc, #8
        ldir
        pop     de
        pop     bc
        inc     c
        djnz    draw8_win0_loop
        pop     af
        out     (#0xE2), a
        ld      a, #0xC0
        out     (#0x89), a
        pop     ix
        ret

_gfx_draw_sprite16_win0::
        push    ix
        ld      iy, #4
        add     iy, sp
        ld      l, 1 (iy)
        ld      h, 2 (iy)
        ld      a, 0 (iy)
        call    _gfx_dest_addr
        ex      de, hl
        ld      l, 4 (iy)
        ld      h, 5 (iy)
        ld      c, 3 (iy)
        in      a, (#0xE2)
        push    af
        ld      a, 6 (iy)
        call    _gfx_page_from_flags
        out     (#0xE2), a
        ld      b, #16
draw16_win0_loop:
        push    bc
        ld      a, c
        out     (#0x89), a
        ld      d, d
        ld      a, #16
        ld      l, l
        ld      a, (hl)
        ld      (de), a
        ld      b, b
        ld      bc, #16
        add     hl, bc
        pop     bc
        inc     c
        djnz    draw16_win0_loop
        pop     af
        out     (#0xE2), a
        ld      a, #0xC0
        out     (#0x89), a
        pop     ix
        ret

_gfx_draw_sprite24_win0::
        push    ix
        ld      iy, #4
        add     iy, sp
        ld      l, 1 (iy)
        ld      h, 2 (iy)
        ld      a, 0 (iy)
        call    _gfx_dest_addr
        ex      de, hl
        ld      l, 4 (iy)
        ld      h, 5 (iy)
        ld      c, 3 (iy)
        in      a, (#0xE2)
        push    af
        ld      a, 6 (iy)
        call    _gfx_page_from_flags
        out     (#0xE2), a
        ld      b, #24
draw24_win0_loop:
        push    bc
        ld      a, c
        out     (#0x89), a
        ld      d, d
        ld      a, #24
        ld      l, l
        ld      a, (hl)
        ld      (de), a
        ld      b, b
        ld      bc, #24
        add     hl, bc
        pop     bc
        inc     c
        djnz    draw24_win0_loop
        pop     af
        out     (#0xE2), a
        ld      a, #0xC0
        out     (#0x89), a
        pop     ix
        ret

; Copy one visible line fragment from any mapped RAM window to VRAM.
; Signature:
;   void gfx_blit_line_accel(u8 screen, u16 x, u8 y,
;                            const void *src, u8 width);
; Preconditions:
;   - WIN3 is already mapped to the desired VRAM page.
;   - interrupts are already disabled by the caller.
;   - width is 1..255.
_gfx_blit_line_accel::
        push    ix
        ld      iy, #4
        add     iy, sp
        ld      l, 1 (iy)
        ld      h, 2 (iy)
        ld      a, 0 (iy)
        call    _gfx_dest_addr
        ex      de, hl
        ld      l, 4 (iy)
        ld      h, 5 (iy)
        ld      a, 3 (iy)
        out     (#0x89), a
        ld      d, d
        ld      a, 6 (iy)
        ld      l, l
        ld      a, (hl)
        ld      (de), a
        ld      b, b
        pop     ix
        ret

_gfx_restore_rect::
        push    ix
        ld      iy, #4
        add     iy, sp
        ld      l, 1 (iy)
        ld      h, 2 (iy)
        ld      a, 0 (iy)
        call    _gfx_dest_addr
        in      a, (#0xE2)
        push    af
        ld      a, #0x50
        out     (#0xE2), a
        ld      c, 3 (iy)
        ld      b, 5 (iy)
7$:
        push    bc
        ld      a, c
        out     (#0x89), a
        di
        ld      d, d
        ld      a, 4 (iy)
        ld      l, l
        ld      a, (hl)
        ld      (hl), a
        ld      b, b
        ei
        pop     bc
        inc     c
        djnz    7$
        pop     af
        out     (#0xE2), a
        ld      a, #0xC0
        out     (#0x89), a
        pop     ix
        ret

; void gfx_copy_rect(u8 dst_screen, u8 src_screen, u16 x, u8 y,
;                    u8 width, u8 height);
;
; Copies a `width`×`height` rectangle from `src_screen` to `dst_screen`
; using the same (x, y) on both.
;
; Two paths:
;   height == 0 (full 256-row column): vertical-copy accelerator — one
;     LD A,A burst per column (`width` iterations). About 20× faster
;     than the row-by-row blit; this is what gfx_copy_screen uses.
;   otherwise: horizontal-copy accelerator — one LD L,L burst per row
;     (`height` iterations × `width` bytes). The vertical-copy command
;     hardwires 256-byte column bursts on this hardware, so it cannot
;     express an arbitrary partial-column rect; fall back to the safe
;     per-row pattern that respects the LD D,D size register.
_gfx_copy_rect::
        push    ix
        ld      iy, #4
        add     iy, sp
        ; HL = src VRAM addr = 0xC000 + src_screen*320 + x
        ld      l, 2 (iy)
        ld      h, 3 (iy)
        ld      a, 1 (iy)
        call    _gfx_dest_addr
        push    hl
        ; DE = dst VRAM addr = 0xC000 + dst_screen*320 + x
        ld      l, 2 (iy)
        ld      h, 3 (iy)
        ld      a, 0 (iy)
        call    _gfx_dest_addr
        ex      de, hl
        pop     hl

        in      a, (#0xE2)
        push    af
        ld      a, #0x50
        out     (#0xE2), a

        ld      a, 6 (iy)         ; height
        or      a
        jr      z, copy_rect_full ; height == 0 → vertical-accel fast path

        ; ----- Per-row horizontal-accel path (arbitrary height) -----
        ; B = height (row counter, djnz)   C = y_start
        ld      c, 4 (iy)
        ld      b, 6 (iy)
copy_rect_row:
        push    bc
        ld      a, c
        out     (#0x89), a        ; PORT_Y = current row
        di
        ld      d, d
        ld      a, 5 (iy)         ; size = width
        ld      l, l              ; horizontal-copy mode
        ld      a, (hl)           ; trigger: read `width` bytes from src
        ld      (de), a           ; trigger: write `width` bytes to dst
        ld      b, b              ; off
        ei
        pop     bc
        inc     c
        djnz    copy_rect_row
        jr      copy_rect_done

        ; ----- Full-column vertical-accel path (height == 256) -----
copy_rect_full:
        ld      a, 4 (iy)
        out     (#0x89), a        ; PORT_Y = y_start
        ld      b, 5 (iy)         ; B = width (column count)

        di
        ld      d, d              ; size-mode
        ld      a, #0             ; size = 0 → 256 bytes per column
                                  ; (LD A,n immediate, the form the accel
                                  ; latches as the new size)
copy_rect_col:
        ld      a, a              ; vertical-copy mode
        ld      a, (hl)           ; trigger: read 256 bytes down src col
        ld      (de), a           ; trigger: write 256 bytes down dst col
        ld      b, b              ; off
        inc     hl                ; next src column
        inc     de                ; next dst column
        djnz    copy_rect_col
        ei

copy_rect_done:
        pop     af
        out     (#0xE2), a
        ld      a, #0xC0
        out     (#0x89), a
        pop     ix
        ret
