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
        .globl  l__BSS
        .globl  s__BSS

        ; ----- Define area ordering -----
        ; CRT0 is linked first, so its _CODE content comes first at 0x8100.
        .area   _CODE

_entry::
        ; Save command line pointer (IX from DSS)
        push    ix
        pop     hl
        ld      (__cmdline), hl

        ; Initialize global/static variables
        call    gsinit

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
