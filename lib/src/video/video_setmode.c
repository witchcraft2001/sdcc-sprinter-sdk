#include <sprinter/video.h>

void video_setmode(u8 mode) __naked {
    /* mode in A. DSS.SetVMod: A=mode, B=video_page */
    __asm
        push    ix
        ld      b, #0           ; video page 0
        ld      c, #0x50
        rst     #0x10
        pop     ix
        ret
    __endasm;
}
