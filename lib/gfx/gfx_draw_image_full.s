        .module gfx_draw_image_full
        .globl  _gfx_draw_image_full_cols
        .globl  _gfx_resource_phys

GFX_PAGE_SIZE       = 0x4000
SCREEN_WIDTH        = 320
COLS_PER_PAGE       = 64
PAGES_PER_FRAME     = 5
VRAM_PAGE_BASE      = 0x50
VRAM_TRANSPARENT    = 0x08
VRAM_ONLY           = 0x04
PORT_WIN0           = 0x82
PORT_WIN3           = 0xE2
PORT_Y              = 0x89

        .area   _CODE

; A = GFX flags, returns A = VRAM page selector for WIN3.
gfx_page_from_flags_full:
        ld      b, a
        ld      a, #VRAM_PAGE_BASE
        bit     0, b
        jr      z, 1$
        add     a, #VRAM_TRANSPARENT
1$:
        bit     1, b
        jr      z, 2$
        add     a, #VRAM_ONLY
2$:
        ret


; ----------------------------------------------------------------------
; void gfx_draw_image_full_cols(u8 screen, u8 page_delta, u8 flags)
;
;   Renders a column-major 320x256 image to (0,0) on `screen`. Source is
;   five consecutive EMM pages, each holding 64 contiguous columns of 256
;   bytes (column c at offset c*256).
;
;   For RAM→VRAM column copy the accelerator needs a two-stage burst per
;   column (see fm/MOD2/KEYBAR.ASM for the canonical pattern):
;     LD D,D / LD A,size / LD B,B          ; commit block size
;     LD L,L / LD A,(HL) / LD B,B          ; horizontal read: 256 bytes
;                                          ; sequential from RAM at (HL)
;                                          ; → accel buffer
;     LD A,A / LD (DE),A / LD B,B          ; vertical write: 256 bytes
;                                          ; from accel buffer down VRAM
;                                          ; column at (DE)
;   `LD A,A` alone is for VRAM→VRAM; using it for the read trigger when
;   the source lives in plain RAM degenerates the operation into a fill
;   with the first source byte.
;
;   iy+0 = screen   iy+1 = page_delta   iy+2 = flags
; ----------------------------------------------------------------------
_gfx_draw_image_full_cols::
        push    ix
        ld      iy, #4
        add     iy, sp

        ; Save WIN3, set destination VRAM mapping from flags
        in      a, (#PORT_WIN3)
        push    af
        ld      a, 2 (iy)
        call    gfx_page_from_flags_full
        out     (#PORT_WIN3), a

        ; Save WIN0
        in      a, (#PORT_WIN0)
        push    af

        ; phys_ptr = &gfx_resource_phys[page_delta]
        ld      a, 1 (iy)
        ld      c, a
        ld      b, #0
        ld      hl, #_gfx_resource_phys
        add     hl, bc
        ld      (cols_phys_ptr), hl

        ; DE = dst base = 0xC000 + (screen & 1) * 320
        ld      a, 0 (iy)
        and     #1
        ld      de, #0xC000
        jr      z, cols_dst_done
        ex      de, hl
        ld      de, #SCREEN_WIDTH
        add     hl, de
        ex      de, hl
cols_dst_done:

        ; Set Y = 0 once; vertical-copy mode handles the row stride
        ; internally between source/dest pairs.
        xor     a
        out     (#PORT_Y), a

        ; Outer loop: 5 source pages × 64 columns each.
        ld      b, #PAGES_PER_FRAME
cols_page_loop:
        push    bc

        ; Map next source page into WIN0
        ld      hl, (cols_phys_ptr)
        ld      a, (hl)
        out     (#PORT_WIN0), a
        inc     hl
        ld      (cols_phys_ptr), hl

        ; HL = 0 (column 0 of this page), B = 64 columns
        ld      hl, #0
        ld      b, #COLS_PER_PAGE

        ; Pin Y to 0 before each page; vertical mode handles internal
        ; advancement but does not rewind across bursts.
        xor     a
        out     (#PORT_Y), a

        di
        ld      d, d             ; size-mode
        ld      a, #0            ; size = 0 → 256 bytes per burst
        ld      b, b             ; off (commits size)

cols_col_loop:
        ld      l, l             ; horizontal-copy mode (RAM source)
        ld      a, (hl)          ; trigger: read 256 RAM bytes into accel
        ld      b, b             ; off
        ld      a, a             ; vertical-copy mode (VRAM dest)
        ld      (de), a          ; trigger: write 256 bytes down VRAM col
        ld      b, b             ; off
        inc     de               ; next VRAM column
        inc     h                ; HL += 256 (next source column in RAM)
        djnz    cols_col_loop

        ei

        pop     bc
        djnz    cols_page_loop

        pop     af
        out     (#PORT_WIN0), a
        pop     af
        out     (#PORT_WIN3), a
        ld      a, #0xC0
        out     (#PORT_Y), a
        pop     ix
        ret


        .area   _DATA

cols_phys_ptr:  .ds 2
