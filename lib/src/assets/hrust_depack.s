        .module hrust_depack
        .globl  _hrust_depack

        .area   _CODE

_hrust_depack::
        push    ix
        ld      iy,#4
        add     iy,sp
        ld      l,0(iy)
        ld      h,1(iy)
        ld      e,2(iy)
        ld      d,3(iy)
        ld      (hrust_saved_sp),sp
        ld      sp,#hrust_stack_top
        call    hrust_depack_hl_de
        ld      sp,(hrust_saved_sp)
        pop     ix
        ret

hrust_depack_hl_de:
        di
        ld      ix,#0xFFF4
        add     ix,sp
        push    de
        ld      sp,hl
        pop     bc
        ex      de,hl
        pop     bc
        dec     bc
        add     hl,bc
        ex      de,hl
        pop     bc
        dec     bc
        add     hl,bc
        sbc     hl,de
        add     hl,de
        jr      c,hrust_8018
        ld      d,h
        ld      e,l
hrust_8018:
        lddr
        ex      de,hl
        ld      d,11(ix)
        ld      e,10(ix)
        ld      sp,hl
        pop     hl
        pop     hl
        pop     hl
        ld      b,#0x06
hrust_8027:
        dec     sp
        pop     af
        ld      6(ix),a
        inc     ix
        djnz    hrust_8027
        exx
        ld      d,#0xBF
        ld      bc,#0x1010
        pop     hl
hrust_8037:
        dec     sp
        pop     af
        exx
hrust_803A:
        ld      (de),a
        inc     de
hrust_803C:
        exx
hrust_803D:
        add     hl,hl
        djnz    hrust_8042
        pop     hl
        ld      b,c
hrust_8042:
        jr      c,hrust_8037
        ld      e,#0x01
hrust_8046:
        ld      a,#0x80
hrust_8048:
        add     hl,hl
        djnz    hrust_804D
        pop     hl
        ld      b,c
hrust_804D:
        rla
        jr      c,hrust_8048
        cp      #0x03
        jr      c,hrust_8059
        add     a,e
        ld      e,a
        xor     c
        jr      nz,hrust_8046
hrust_8059:
        add     a,e
        cp      #0x04
        jr      z,hrust_80B8
        adc     a,#0xFF
        cp      #0x02
        exx
hrust_8063:
        ld      c,a
hrust_8064:
        exx
        ld      a,#0xBF
        jr      c,hrust_807D
hrust_8069:
        add     hl,hl
        djnz    hrust_806E
        pop     hl
        ld      b,c
hrust_806E:
        rla
        jr      c,hrust_8069
        jr      z,hrust_8078
        inc     a
        add     a,d
        jr      nc,hrust_807F
        sub     d
hrust_8078:
        inc     a
        jr      nz,hrust_8087
        ld      a,#0xEF
hrust_807D:
        rrca
        cp      a
hrust_807F:
        add     hl,hl
        djnz    hrust_8084
        pop     hl
        ld      b,c
hrust_8084:
        rla
        jr      c,hrust_807F
hrust_8087:
        exx
        ld      h,#0xFF
        jr      z,hrust_8092
        ld      h,a
        dec     sp
        inc     a
        jr      z,hrust_809D
        pop     af
hrust_8092:
        ld      l,a
        add     hl,de
        ldir
hrust_8096:
        jr      hrust_803C
hrust_8098:
        exx
        rrc     d
        jr      hrust_803D
hrust_809D:
        pop     af
        cp      #0xE0
        jr      c,hrust_8092
        rlca
        xor     c
        inc     a
        jr      z,hrust_8098
        sub     #0x10
hrust_80A9:
        ld      l,a
        ld      c,a
        ld      h,#0xFF
        add     hl,de
        ldi
        dec     sp
        pop     af
        ld      (de),a
        inc     hl
        inc     de
        ld      a,(hl)
        jr      hrust_803A
hrust_80B8:
        ld      a,#0x80
hrust_80BA:
        add     hl,hl
        djnz    hrust_80BF
        pop     hl
        ld      b,c
hrust_80BF:
        adc     a,a
        jr      nz,hrust_80DB
        jr      c,hrust_80BA
        ld      a,#0xFC
        jr      hrust_80DE
hrust_80C8:
        dec     sp
        pop     bc
        ld      c,b
        ld      b,a
        ccf
        jr      hrust_8064
hrust_80CF:
        cp      #0x0F
        jr      c,hrust_80C8
        jr      nz,hrust_8063
        add     a,#0xF4
        ld      sp,ix
        jr      hrust_80EF
hrust_80DB:
        sbc     a,a
        ld      a,#0xEF
hrust_80DE:
        add     hl,hl
        djnz    hrust_80E3
        pop     hl
        ld      b,c
hrust_80E3:
        rla
        jr      c,hrust_80DE
        exx
        jr      nz,hrust_80A9
        bit     7,a
        jr      z,hrust_80CF
        sub     #0xEA
hrust_80EF:
        ex      de,hl
hrust_80F0:
        pop     de
        ld      (hl),e
        inc     hl
        ld      (hl),d
        inc     hl
        dec     a
        jr      nz,hrust_80F0
        ex      de,hl
        jr      nc,hrust_8096
        ret

        .area   _DATA

hrust_saved_sp:
        .dw     #0x0000
hrust_stack:
        .ds     32
hrust_stack_top:
