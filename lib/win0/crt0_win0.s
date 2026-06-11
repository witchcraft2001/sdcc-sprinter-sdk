; =========================================================================
;  crt0_win0.s - C runtime for the extended WIN0..WIN2 layout (SDK runtime).
;  (see docs/ru/11_extended_layout.md)
;
;  Entry at 0x0300 (first in _CODE), reached from the stage-1 loader with:
;    - WIN0 = our code page P0,
;    - WIN2 = still the loader page (we switch it to the private P2 below),
;    - boot params at P0:0x0040 = { dss_page, p0, p1, p2 }.
;
;  We give the program a PRIVATE WIN2 page P2 (full 0x8000-0xBFFF): set WIN2=P2,
;  move the stack into it, run gsinit, install the RST trampolines, enable
;  interrupts and call main(). All BIOS/DSS/IM1 access goes through the WIN2
;  trampolines installed into P0:0x0008/0x0010/0x0038.
; =========================================================================

        .module crt0_win0
        .globl  _main
        .globl  l__INITIALIZER
        .globl  s__INITIALIZED
        .globl  s__INITIALIZER
        .globl  l__DATA
        .globl  s__DATA
        .globl  l__BSS
        .globl  s__BSS
        .globl  _win0_install_vectors
        .globl  __cmdline
        .globl  _wrt_dss
        .globl  _wrt_p0
        .globl  _wrt_p1
        .globl  _wrt_p2

        .area   _CODE
_win0_entry::
        di
        ; --- map WIN1 = P1 (multi-page code) and WIN2 = private P2 ---
        ld      a, (#0x0042)        ; p1 (boot param, WIN0 = P0); 0 if single-page
        out     (#0xA2), a          ; WIN1 = P1
        ld      a, (#0x0043)        ; p2
        out     (#0xC2), a          ; WIN2 = P2
        ld      sp, #0xBF00         ; stack in private WIN2

        call    gsinit

        ; --- copy boot params (P0:0x0040) into the WIN2 page table ---
        ld      a, (#0x0040)
        ld      (_wrt_dss), a
        ld      a, (#0x0041)
        ld      (_wrt_p0), a
        ld      a, (#0x0042)
        ld      (_wrt_p1), a
        ld      a, (#0x0043)
        ld      (_wrt_p2), a

        ; --- install RST vectors into P0:0x0000-0x003F ---
        call    _win0_install_vectors

        ; --- command line: loader copied {len,text} to P0:0x0080 ---
        ld      hl, #0x0080
        ld      (__cmdline), hl     ; dss_cmdline() returns __cmdline+1

        ei
        call    _main

        ; main() returned -> DSS.Exit via the RST10 trampoline
        ld      a, l
        ld      b, l
        ld      c, #0x41
        rst     #0x10
        ; (does not return)

        .area   _HOME
        .area   _INITIALIZER
        .area   _GSINIT
        .area   _GSFINAL
        .area   _DATA
        .area   _INITIALIZED
        .area   _BSEG
        .area   _BSS
        .area   _HEAP

        .area   _GSINIT
gsinit::
        ld      hl, #s__DATA
        ld      bc, #l__DATA
        ld      a, b
        or      a, c
        jr      z, gsw_copy
        ld      (hl), #0
        dec     bc
        ld      a, b
        or      a, c
        jr      z, gsw_copy
        ld      d, h
        ld      e, l
        inc     de
        ldir
gsw_copy:
        ld      bc, #l__INITIALIZER
        ld      a, b
        or      a, c
        jr      z, gsw_bss
        ld      de, #s__INITIALIZED
        ld      hl, #s__INITIALIZER
        ldir
gsw_bss:
        ld      hl, #s__BSS
        ld      bc, #l__BSS
        ld      a, b
        or      a, c
        jr      z, gsw_done
        ld      (hl), #0
        dec     bc
        ld      a, b
        or      a, c
        jr      z, gsw_done
        ld      d, h
        ld      e, l
        inc     de
        ldir
gsw_done:
        .area   _GSFINAL
        ret

        .area   _DATA
__cmdline::
        .ds     2                   ; pointer to {len,text} cmd line (= 0x0080)
