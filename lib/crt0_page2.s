; =========================================================================
;  crt0_page2.s — SDCC C runtime with DSS-allocated slot2 DATA/stack page
; =========================================================================
;  Use with EXE header SP in slot1 below PSP, typically:
;    CODE_LOC = 0x4200
;    STACK    = 0x40FF
;
;  DSS creates PSP at LD_ADDR-0x0100, so 0x4100..0x41FF is PSP and
;  0x4000..0x40FF remains available as a bootstrap stack. Runtime then
;  allocates one DSS page, maps it to slot2 and moves the working stack to
;  0xBFFF before touching DATA/BSS.
; =========================================================================

        .module crt0_page2
        .globl  _main
        .globl  _dss_exit
        .globl  _dss_exit_safe_sp
        .globl  l__INITIALIZER
        .globl  s__INITIALIZED
        .globl  s__INITIALIZER
        .globl  l__DATA
        .globl  s__DATA
        .globl  l__BSS
        .globl  s__BSS

        .area   _CODE

_entry::
        ; Preserve DSS IX command line pointer on the bootstrap stack.
        push    ix

        ; dss_exit() switches back here before DSS.LEAVE frees process pages.
        ld      hl, #0
        add     hl, sp
        ld      (_dss_exit_safe_sp), hl

        ; Allocate a 16K process page for slot2 DATA and the working stack.
        ld      b, #1
        ld      c, #0x3D        ; DSS.GETMEM
        rst     #0x10
        jr      nc, crt0_page2_gotmem

        ld      b, #1
        ld      c, #0x41        ; DSS.EXIT, not enough memory
        rst     #0x10

crt0_page2_gotmem:
        ld      b, #0
        ld      c, #0x3A        ; DSS.SETWIN2
        rst     #0x10

        pop     ix
        ld      sp, #0xBFFF
        push    ix

        call    gsinit

        pop     hl
        ld      (__cmdline), hl

        call    _main

        ld      h, #0
        push    hl
        call    _dss_exit

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
        ld      bc, #l__INITIALIZER
        ld      a, b
        or      a, c
        jr      z, gsinit_clear_bss

        ld      de, #s__INITIALIZED
        ld      hl, #s__INITIALIZER
        ldir

gsinit_clear_bss:
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

        .area   _DATA
__cmdline::
        .ds     2
