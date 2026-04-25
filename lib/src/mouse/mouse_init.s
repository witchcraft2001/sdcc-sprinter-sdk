        .module mouse_init
        .globl  _mouse_init

        .area   _CODE

_mouse_init::
        push    ix
        ld      c, #0x00
        rst     #0x30
        pop     ix
        ld      hl, #0x0000
        jr      c, _mouse_init_no_290
        inc     l
_mouse_init_no_290:
        ret
