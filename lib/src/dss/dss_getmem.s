        .module dss_getmem
        .globl  _dss_getmem
        .globl  _dss_getmem_pages

        .area   _CODE

_dss_getmem::
        push    ix
        ld      b,#1
        jr      getmem_call

_dss_getmem_pages::
        push    ix
        ld      iy,#4
        add     iy,sp
        ld      b,0(iy)
getmem_call:
        ld      c,#0x3D
        rst     #0x10
        pop     ix
        jr      c,1$
        ld      l,a
        ld      h,#0x00
        ret
1$:
        ld      hl,#0x00FF
        ret
