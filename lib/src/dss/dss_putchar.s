        .module dss_putchar
        .globl  _dss_putchar

        .area   _CODE

_dss_putchar::
        push    ix
        ld      iy, #4
        add     iy, sp
        ld      a, 0 (iy)
        cp      #0x0A
        jr      nz, _dss_pc_out_290
        push    af
        ld      a, #0x0D
        ld      c, #0x5B
        rst     #0x10
        pop     af
_dss_pc_out_290:
        ld      c, #0x5B
        rst     #0x10
        pop     ix
        ret
