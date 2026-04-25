        .module mouse_show
        .globl  _mouse_show

        .area   _CODE

_mouse_show::
        push    ix
        ld      c,#0x01
        rst     #0x30
        pop     ix
        ret
