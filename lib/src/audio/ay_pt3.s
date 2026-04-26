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
; Input:
;   IX = SDCC stack frame with block id at 4(ix)
;   HL = entry offset from 0xC000
; Return:
;   HL = 0 on success, DSS SETWIN error code otherwise
ay_pt3_call:
        in      a,(#0xE2)
        push    af
        push    hl

        ; Map the first page of the DSS block to WIN3.
        ld      a,4(ix)
        ld      b,#0x00
        ld      c,#0x3B
        rst     #0x10
        jr      c,ay_pt3_error

        pop     hl
        di
        call    ay_pt3_do_call
        ei

        pop     af
        out     (#0xE2),a
        ld      hl,#0x0000
        ret

ay_pt3_error:
        ld      e,a
        pop     hl
        pop     af
        out     (#0xE2),a
        ld      l,e
        ld      h,#0x00
        ret

ay_pt3_do_call:
        jp      (hl)

_ay_pt3_init::
        push    ix
        ld      ix,#0
        add     ix,sp
        ld      hl,#0xC000
        call    ay_pt3_call
        pop     ix
        ret

_ay_pt3_play::
        push    ix
        ld      ix,#0
        add     ix,sp
        ld      hl,#0xC005
        call    ay_pt3_call
        pop     ix
        ret

_ay_pt3_mute::
        push    ix
        ld      ix,#0
        add     ix,sp
        ld      hl,#0xC008
        call    ay_pt3_call
        pop     ix
        ret
