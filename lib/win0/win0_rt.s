; =========================================================================
;  win0_rt.s - WIN0..WIN2 runtime: RST trampolines + vector installer.
;  Linked into _WINRT at 0xA000 (WIN2, never repaged). Page-table vars in
;  _DATA (WIN2). See docs/ru/11_extended_layout.md (11.8).
;
;  Each trampoline swaps the DSS core page into WIN0 for the firmware call,
;  then restores our code page P0. The IM1 trampoline routes the frame INT
;  to the DSS handler and returns to restore WIN0.
; =========================================================================

        .module win0_rt
        .globl  _rst08_tramp
        .globl  _rst10_tramp
        .globl  _rst38_tramp
        .globl  _win0_install_vectors
        .globl  _wrt_dss
        .globl  _wrt_p0
        .globl  _wrt_p1
        .globl  _wrt_p2

        .area   _WINRT

; Trampoline rules:
;  * Only WIN0 is switched to the DSS core page; ROM paging for BIOS calls is
;    done by that page's own RST_0x08 stub (#7C/#3C), so we never touch #3C.
;  * The caller's interrupt state is PRESERVED, not forced: sample IFF on entry
;    (LD A,I -> P/V) and re-enable INTs only if the caller had them enabled. Some
;    BIOS calls require DI on entry; the trampoline must not implicitly enable
;    INTs. We carry the decision with self-modifying code (the trampoline lives in
;    WIN2 RAM): patch the in-place EI/NOP opcode bytes (0xFB / 0x00) instead of a
;    data cell + branch -- smaller and faster. (LD A,I has the known Z80 erratum
;    if an INT lands exactly during it; at worst INTs stay disabled for one call.)

; --- BIOS (RST #08) ---
_rst08_tramp::
        push    af                  ; save caller AF (A may be a param)
        ld      a, i                ; P/V = caller IFF2
        di                          ; protect the WIN0 swap
        ld      a, #0x00            ; NOP opcode (caller had INTs disabled)
        jp      po, 1$              ; PO => IFF2 was 0 -> keep NOP
        ld      a, #0xFB            ; EI opcode (caller had INTs enabled)
1$:
        ld      (2$), a             ; SMC: patch pre-call EI/NOP
        ld      (3$), a             ; SMC: patch post-call EI/NOP
        ld      a, (_wrt_dss)
        out     (#0x82), a          ; WIN0 = DSS core page
        pop     af                  ; restore caller AF
2$:
        nop                         ; -> EI/NOP: restore caller INT state for the call
        rst     #0x08
        di
        push    af                  ; save return AF (A + CF)
        ld      a, (_wrt_p1)
        out     (#0xA2), a          ; WIN1 = P1
        ld      a, (_wrt_p0)
        out     (#0x82), a          ; WIN0 = P0
        pop     af
3$:
        nop                         ; -> EI/NOP: restore caller INT state
        ret

; --- DSS (RST #10) ---
_rst10_tramp::
        push    af
        ld      a, i                ; P/V = caller IFF2
        di
        ld      a, #0x00
        jp      po, 1$
        ld      a, #0xFB
1$:
        ld      (2$), a             ; SMC: patch pre-call EI/NOP
        ld      (3$), a             ; SMC: patch post-call EI/NOP
        ld      a, (_wrt_dss)
        out     (#0x82), a          ; WIN0 = DSS core page
        pop     af
2$:
        nop                         ; -> EI/NOP
        rst     #0x10
        di
        push    af
        ld      a, (_wrt_p1)
        out     (#0xA2), a          ; WIN1 = P1
        ld      a, (_wrt_p0)
        out     (#0x82), a          ; WIN0 = P0
        pop     af
3$:
        nop                         ; -> EI/NOP
        ret

; --- IM1 (RST #38) ---
_rst38_tramp::
        push    af
        push    hl
        ld      a, (_wrt_dss)
        out     (#0x82), a          ; WIN0 = DSS core page (0x0038 = .Handler)
        ld      hl, #win0_im1_resume
        push    hl                  ; DSS .Handler RETI lands here
        ld      hl, #0x0038
        jp      (hl)
win0_im1_resume:
        di
        ld      a, (_wrt_p1)
        out     (#0xA2), a          ; WIN1 = P1
        ld      a, (_wrt_p0)
        out     (#0x82), a          ; WIN0 = P0
        pop     hl
        pop     af
        ei
        reti

; --- install JP stubs at P0:0x0008/0x0010/0x0038 (WIN0 = P0) ---
_win0_install_vectors::
        ld      hl, #0x0008
        ld      (hl), #0xC3
        inc     hl
        ld      de, #_rst08_tramp
        ld      a, e
        ld      (hl), a
        inc     hl
        ld      a, d
        ld      (hl), a

        ld      hl, #0x0010
        ld      (hl), #0xC3
        inc     hl
        ld      de, #_rst10_tramp
        ld      a, e
        ld      (hl), a
        inc     hl
        ld      a, d
        ld      (hl), a

        ld      hl, #0x0038
        ld      (hl), #0xC3
        inc     hl
        ld      de, #_rst38_tramp
        ld      a, e
        ld      (hl), a
        inc     hl
        ld      a, d
        ld      (hl), a
        ret

        .area   _DATA
_wrt_dss::
        .ds     1
_wrt_p0::
        .ds     1
_wrt_p1::
        .ds     1
_wrt_p2::
        .ds     1
