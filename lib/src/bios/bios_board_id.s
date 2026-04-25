        .module bios_board_id
        .globl  _bios_board_id

        .area   _CODE

_bios_board_id::
        push    ix
        ld      c, #0xED
        rst     #0x08
        pop     ix
        ld      l, a
        ld      h, #0
        ret
