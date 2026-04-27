; =========================================================================
;  crt0.s — SDCC C Runtime for ZX Sprinter / Estex DSS
; =========================================================================
;  Memory layout:
;    0x8000-0x80FF  EXE header (on disk only, not loaded into memory)
;    0x8100         Program entry point (code + data)
;    0xBFFF         Initial stack pointer (grows downward)
;
;  DSS loads the program at 0x8100 and jumps to the entry point.
;  The command line string pointer is passed in IX by DSS.
;
;  IMPORTANT: crt0.rel MUST be the first .rel file passed to the linker
;  so that _entry is placed at the start of _CODE (address 0x8100).
; =========================================================================

        .module crt0
        .globl  _main
        .globl  l__INITIALIZER
        .globl  s__INITIALIZED
        .globl  s__INITIALIZER
        .globl  l__DATA
        .globl  s__DATA
        .globl  l__BSS
        .globl  s__BSS

        ; ----- Define area ordering -----
        ; CRT0 is linked first, so its _CODE content comes first at 0x8100.
        .area   _CODE

_entry::
        ; Preserve command line pointer (IX from DSS) across runtime init.
        push    ix

        ; Initialize global/static variables
        call    gsinit

        ; Save command line pointer after DATA clear/init.
        pop     hl
        ld      (__cmdline), hl

        ; Call main()
        call    _main

        ; main() returned — exit with return code in L
        ld      a, l
        ld      b, l
        ld      c, #0x41        ; DSS.Exit
        rst     #0x10
        ; (does not return)

        ; ----- Remaining area ordering -----
        .area   _HOME
        .area   _INITIALIZER
        .area   _GSINIT
        .area   _GSFINAL

        .area   _DATA
        .area   _INITIALIZED
        .area   _BSEG
        .area   _BSS
        .area   _HEAP

        ; ----- Global initialization -----
        .area   _GSINIT
gsinit::
        ; SDCC 2.9 places uninitialized globals/statics in DATA.
        ; Clear DATA first; generated GSINIT code restores initialized values.
        ld      hl, #s__DATA
        ld      bc, #l__DATA
        ld      a, b
        or      a, c
        jr      z, gsinit_copy_init

        ld      (hl), #0
        dec     bc
        ld      a, b
        or      a, c
        jr      z, gsinit_copy_init

        ld      d, h
        ld      e, l
        inc     de
        ldir

gsinit_copy_init:
        ; Copy INITIALIZER -> INITIALIZED (initialized globals)
        ld      bc, #l__INITIALIZER
        ld      a, b
        or      a, c
        jr      z, gsinit_clear_bss

        ld      de, #s__INITIALIZED
        ld      hl, #s__INITIALIZER
        ldir

gsinit_clear_bss:
        ; Clear BSS (zero-initialized globals)
        ld      hl, #s__BSS
        ld      bc, #l__BSS
        ld      a, b
        or      a, c
        jr      z, gsinit_done

        ld      (hl), #0
        dec     bc
        ld      a, b
        or      a, c
        jr      z, gsinit_done

        ld      d, h
        ld      e, l
        inc     de
        ldir

gsinit_done:

        .area   _GSFINAL
        ret

        ; ----- Data -----
        .area   _DATA
__cmdline::
        .ds     2               ; saved command line pointer
