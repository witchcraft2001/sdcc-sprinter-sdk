        .module bios_emm_list
        .globl  _bios_emm_list

        .area   _CODE

_bios_emm_list::
        push    ix
        ld      iy,#4
        add     iy,sp
        ld      a,0(iy)
        ld      l,1(iy)
        ld      h,2(iy)
        ld      c,#0xC5
        rst     #0x08
        pop     ix
        ld      l,b
        ld      h,#0x00
        ret
