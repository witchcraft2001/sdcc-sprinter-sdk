        .module dss_callp
        .globl  _dss_callp

        .area   _CODE

_dss_callp::
        push    ix
        ld      iy,#4
        add     iy,sp
        ld      l,0(iy)
        ld      h,1(iy)
        ld      e,2(iy)
        ld      d,3(iy)
        push    de
        call    1$
        pop     bc
        pop     ix
        ret
1$:
        jp      (hl)
