        .module dss_call
        .globl  _dss_call

        .area   _CODE

_dss_call::
        push    ix
        ld      iy,#4
        add     iy,sp
        ld      l,0(iy)
        ld      h,1(iy)
        call    1$
        pop     ix
        ret
1$:
        jp      (hl)
