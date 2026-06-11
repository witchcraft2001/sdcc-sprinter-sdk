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

; --- BIOS (RST #08) ---
_rst08_tramp::
        di
        push    af
        xor     a
        out     (#0x3C), a          ; ROM overlay off
        ld      a, (_wrt_dss)
        out     (#0x82), a          ; WIN0 = DSS core page
        pop     af
        ei
        rst     #0x08
        di
        push    af
        ld      a, (_wrt_p0)
        out     (#0x82), a          ; WIN0 = P0
        pop     af
        ei
        ret

; --- DSS (RST #10) ---
_rst10_tramp::
        di
        push    af
        xor     a
        out     (#0x3C), a
        ld      a, (_wrt_dss)
        out     (#0x82), a          ; WIN0 = DSS core page
        pop     af
        ei
        rst     #0x10
        di
        push    af
        ld      a, (_wrt_p0)
        out     (#0x82), a          ; WIN0 = P0
        pop     af
        ei
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
