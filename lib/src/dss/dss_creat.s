        .module dss_creat
        .globl  _dss_creat

        .area   _CODE

_dss_creat::
        push    ix
        ld      iy,#4
        add     iy,sp
        ld      l,0(iy)
        ld      h,1(iy)
        ld      a, #0x20
        ld      c, #0x0A
        rst     #0x10
        pop     ix
        jr      c, _dss_creat_err_290
        ld      l, a
        ld      h, #0
        ret
_dss_creat_err_290:
        ld      hl, #0xFFFF
        ret
