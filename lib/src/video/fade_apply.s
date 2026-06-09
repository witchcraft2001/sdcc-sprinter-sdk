        .module fade_apply
        .globl  _fade_apply_step

;; ---------------------------------------------------------------------
;; void fade_apply_step(const video_rgb8_t *src_rgb8, const u8 *lut)
;;
;; SDCC 2.9.0 uses stack-based calling (sdcccall(0)). After our two
;; push ix / push iy, the stack frame is:
;;
;;   SP+0..+1 = saved IY
;;   SP+2..+3 = saved IX
;;   SP+4..+5 = return address
;;   SP+6..+7 = src_rgb8 (1st arg)
;;   SP+8..+9 = lut      (2nd arg)
;;
;; Direct palette write through WIN3 + $C3E0..$C3E2. Protocol confirmed
;; by mame/src/mame/sinclair/sprinter.cpp and on-hardware diagnostics
;; (examples/30_paltest, test 1: WIN3 + $C3E0):
;;   - VRAM #50 maps as 256 rows * 1024 bytes; PORT_Y selects row,
;;     CPU window doesn't matter; offsets $..3E0..$..3E2 within row
;;     decode to palette[PORT_Y] R/G/B.
;;   - PORT_Y (port #89) is OUT-only -- IN returns CBL state, NOT
;;     PORT_Y. We keep the index in register E and OUT it directly.
;; ---------------------------------------------------------------------

PORT_Y          = 0x89
PORT_WIN3       = 0xE2
VRAM_PAGE_50    = 0x50
PAL_R           = 0xC3E0
PAL_G           = 0xC3E1
PAL_B           = 0xC3E2

        .area   _CODE

_fade_apply_step::
        push    ix
        push    iy

        ;; --- Load src_rgb6 (1st arg) into IX ---
        ld      hl, #6
        add     hl, sp
        ld      a, (hl)
        inc     hl
        ld      h, (hl)
        ld      l, a
        push    hl
        pop     ix                      ; IX = src_rgb8

        ;; --- Load lut (2nd arg) into BC ---
        ld      hl, #8
        add     hl, sp
        ld      a, (hl)
        inc     hl
        ld      h, (hl)
        ld      c, a
        ld      b, h                    ; BC = lut

        in      a, (#PORT_WIN3)
        ld      (fap_saved_win3), a

        ld      a, #VRAM_PAGE_50
        out     (#PORT_WIN3), a         ; WIN3 = VRAM #50 (BIOS in WIN0 stays mapped, no DI needed)

        ld      e, #0                   ; PORT_Y counter (0..255)

        exx
        ld      b, #0                   ; alt B' = 256 djnz iterations
        exx

fap_loop:
        ld      a, e
        out     (#PORT_Y), a            ; PORT_Y = E

        ;; --- R = lut[ src[i].r ] -> $C3E0 ---
        ld      a, 0 (ix)
        ld      l, a
        ld      h, #0
        add     hl, bc
        ld      a, (hl)
        ld      (#PAL_R), a

        ;; --- G = lut[ src[i].g ] -> $C3E1 ---
        ld      a, 1 (ix)
        ld      l, a
        ld      h, #0
        add     hl, bc
        ld      a, (hl)
        ld      (#PAL_G), a

        ;; --- B = lut[ src[i].b ] -> $C3E2 ---
        ld      a, 2 (ix)
        ld      l, a
        ld      h, #0
        add     hl, bc
        ld      a, (hl)
        ld      (#PAL_B), a

        inc     ix
        inc     ix
        inc     ix
        inc     e

        exx
        djnz    fap_alt
        exx
        jr      fap_done
fap_alt:
        exx
        jr      fap_loop

fap_done:
        ld      a, #0xC0
        out     (#PORT_Y), a            ; PORT_Y safe zone

        ld      a, (fap_saved_win3)
        out     (#PORT_WIN3), a         ; restore WIN3 (e.g. PT3 player at #C000)

        pop     iy
        pop     ix
        ret


        .area   _DATA

fap_saved_win3:
        .ds     1
