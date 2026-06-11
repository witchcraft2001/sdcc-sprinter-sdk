        .module ay_pt3
        .globl  _ay_pt3_init
        .globl  _ay_pt3_play
        .globl  _ay_pt3_mute

        .area   _CODE

; Call a PT3 player image mapped into WIN3.
; The image must be assembled for 0xC000:
;   0xC000 START   - init, module follows player
;   0xC005 START+5 - play one frame
;   0xC008 START+8 - mute
;
; ABI: SDCC 4.5.0 __sdcccall(1). The block id (1st u8 arg) arrives in A; the
; entry offset is passed in HL by the small per-entry stubs below. The u8
; return value is left in A.
;
; Input:  A = DSS block id, HL = entry offset (0xC000 / 0xC005 / 0xC008)
; Return: A = 0 on success, DSS SETWIN error code otherwise
;
; IX/IY are preserved across the (external) player call; the caller's WIN3
; mapping is saved on entry and restored before return.
ay_pt3_call:
        push    ix
        push    iy
        ld      b, a                ; B = block id (A is about to be reused)
        in      a, (#0xE2)          ; A = current WIN3 mapping
        push    af                  ; save WIN3
        push    hl                  ; save entry offset

        ; Map the first page of the DSS block to WIN3 (SETWIN win=3, page 0).
        ld      a, b                ; A = block id
        ld      b, #0x00
        ld      c, #0x3B
        rst     #0x10
        jr      c, ay_pt3_error

        pop     hl                  ; HL = entry offset
        di
        call    ay_pt3_do_call
        ei

        pop     af                  ; A = saved WIN3
        out     (#0xE2), a
        pop     iy
        pop     ix
        xor     a                   ; success -> 0
        ret

ay_pt3_error:
        ; A = SETWIN error code (CF set). Stack top: [entry offset][saved WIN3].
        pop     hl                  ; discard entry offset
        ld      h, a                ; H = error code
        pop     af                  ; A = saved WIN3
        out     (#0xE2), a
        ld      a, h                ; A = error code
        pop     iy
        pop     ix
        ret

ay_pt3_do_call:
        jp      (hl)

_ay_pt3_init::
        ld      hl, #0xC000
        jr      ay_pt3_call

_ay_pt3_play::
        ld      hl, #0xC005
        jr      ay_pt3_call

_ay_pt3_mute::
        ld      hl, #0xC008
        jr      ay_pt3_call
