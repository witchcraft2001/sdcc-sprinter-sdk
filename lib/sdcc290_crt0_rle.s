        ;; SDCC 2.9.0 RLE init helper for global data.
        ;; Derived from the historical SDCC z80 runtime.

        .area   _CODE

__initrleblock::
        ld      c,l
        ld      b,h

        pop     hl
1$:
        ld      e,(hl)
        inc     hl
        bit     7,e
        jp      Z,2$
        ld      a,(hl)
        inc     hl
3$:
        ld      (bc),a
        inc     bc
        inc     e
        jp      NZ,3$
        jp      1$
2$:
        xor     a
        or      e
        jp      Z,4$
5$:
        ld      a,(hl)
        inc     hl
        ld      (bc),a
        inc     bc
        dec     e
        jp      NZ,5$
        jp      1$
4$:
        push    hl
        ret
