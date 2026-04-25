        .module mouse_hide
        .globl  _mouse_hide

        .area   _CODE

_mouse_hide::
        push    ix
        ld      c,#0x02
        rst     #0x30
        pop     ix
        ret
