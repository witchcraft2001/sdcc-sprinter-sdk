        .module dss_appinfo
        .globl  _dss_appinfo

        .area   _CODE

_dss_appinfo::
        push    ix
        ld      iy,#4
        add     iy,sp
        ld      b,0(iy)      ; subfunc -> B
        ; DSS #47 documents the buffer in HL (handler does EX DE,HL).
        ; Estex-DSS R10 leaves the EX commented out for subfunc 0 (PARAMS),
        ; so that path expects the buffer in DE instead. Load buf into BOTH
        ; HL and DE so all three subfuncs work on Estex-DSS and the canonical
        ; Dss1 handler alike.
        ld      l,1(iy)      ; buf -> HL  (PATH / FULLNAME)
        ld      h,2(iy)
        ld      e,l          ; buf -> DE  (PARAMS on Estex-DSS R10)
        ld      d,h
        ld      c,#0x47
        rst     #0x10
        pop     ix
        jr      c,1$
        ld      hl,#0x0000
        ret
1$:
        ld      hl,#0xFFFF
        ret
