        .module fade_pal
        .globl  _video_pal_accel_push

;; ---------------------------------------------------------------------
;; void video_pal_accel_push(const u8 *src_channels)
;;
;; SDCC sdcccall(1): HL = src_channels (768 bytes laid out as three
;; 256-byte channels: B[0..255], G[0..255], R[0..255], components
;; already shifted into the high bits = << 2).
;;
;; Pushes all 256 palette entries into BOTH hardware palette pages
;; using the Sprinter accelerator's "horizontal read + vertical write"
;; burst mode. This is the lib_startup im2handler pattern from
;; /Users/dmitry/dev/zx/sprinter/evo_sdk/sprintersdk/lib_startup.asm
;; (lines 506-547), adapted to use WIN3 instead of WIN1 because the
;; SDK's default CODE_LOC = 0x4100 places our code in WIN1 - so we
;; remap WIN3 (#E2) to VRAM page #50 and address the palette
;; registers as $C3E0 (page 0) / $C3E4 (page 1) instead of $43E0 /
;; $43E4.
;;
;; Six accel bursts total: three channels (B, G, R) x two palette
;; pages. Each burst transfers 256 bytes from RAM through the accel
;; with PORT_Y auto-stepping 0..255, so one burst sets one byte
;; position for ALL 256 palette entries.
;;
;; Accel opcodes used (single-byte NOPs to a normal Z80, special to
;; the Sprinter FPGA - see manual/05_graphics/06_accelerator.md):
;;   ld d, d  (0x52)  - size-mode
;;   ld l, l  (0x6D)  - horizontal read mode (RAM source)
;;   ld a, a  (0x7F)  - vertical write mode (VRAM dest, advances Y)
;;   ld b, b  (0x40)  - off / commit
;; ---------------------------------------------------------------------

        .area   _CODE

_video_pal_accel_push::
        push    ix
        push    iy

        in      a, (#0xE2)              ; save WIN3 mapping
        ex      af, af'                 ; -> AF'

        di
        ld      a, #0x50
        out     (#0xE2), a              ; map VRAM #50 into WIN3

        ; --- Page 0: BC = $C3E0, DE = src start ---
        push    hl                      ; save src for page 1 pass
        ex      de, hl                  ; DE = src start
        ld      bc, #0xC3E0
        call    pap_burst3

        ; --- Page 1: BC = $C3E4, DE = src start (re-read same source) ---
        pop     hl
        ex      de, hl
        ld      bc, #0xC3E4
        call    pap_burst3

        ld      a, #0xC0
        out     (#0x89), a              ; PORT_Y safe

        ex      af, af'
        out     (#0xE2), a              ; restore WIN3

        ei
        pop     iy
        pop     ix
        ret

;; --- Internal: three accel bursts (B, G, R channels) ---
;; DE = src start, BC = dst base ($C3E0 or $C3E4)
;; After return: DE += 768, BC += 3, PORT_Y left at some intermediate value.
pap_burst3:
        ; Burst 1: B channel
        xor     a
        out     (#0x89), a
        ld      d, d
        ld      a, #0
        ld      l, l
        ld      a, (de)
        ld      a, a
        ld      (bc), a
        ld      b, b
        inc     d
        inc     bc

        ; Burst 2: G channel
        xor     a
        out     (#0x89), a
        ld      d, d
        ld      a, #0
        ld      l, l
        ld      a, (de)
        ld      a, a
        ld      (bc), a
        ld      b, b
        inc     d
        inc     bc

        ; Burst 3: R channel
        xor     a
        out     (#0x89), a
        ld      d, d
        ld      a, #0
        ld      l, l
        ld      a, (de)
        ld      a, a
        ld      (bc), a
        ld      b, b
        inc     d
        inc     bc

        ret
