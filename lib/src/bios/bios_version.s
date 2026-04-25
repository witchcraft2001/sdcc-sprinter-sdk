        .module bios_version
        .globl  _bios_version

        .area   _CODE

_bios_version::
        push    ix
        ld      c, #0xEE
        rst     #0x08
        pop     ix
        ex      de, hl
        ret
