#include <sprinter/bios.h>
#include <sprinter/ports.h>

/*
 * bios_setpal - set one palette entry via BIOS PIC_SET_PAL (#A4).
 *
 * Ported from the reference SDK's fixed bios_setpal: the hardware palette
 * format is 8-bit per channel in B, G, R order, and BIOS #A4 must be called
 * for BOTH palette pages (0 and 1) so the colour is correct across double
 * buffering. This replaces the earlier direct port-0x89 path (which predated
 * the reference's channel-order / 8-bit fixes).
 *
 * SDCC 4.5.0 __sdcccall(1): index=A, r=L, g & b on the stack (2 bytes,
 * callee-cleaned). A 6-byte scratch frame holds the stashed index plus the
 * [B,G,R,0] buffer that BIOS #A4 reads through HL.
 */
void bios_setpal(u8 index, u8 r, u8 g, u8 b) __naked {
    (void)index; (void)r; (void)g; (void)b;
    __asm
        push    ix
        ld      ix, #0
        add     ix, sp          ; IX = frame: 4(ix)=g, 5(ix)=b (caller stack args)
        ld      c, a            ; C = index
        ld      b, l            ; B = r

        ; allocate 6-byte scratch: [index][pad][B][G][R][0] at IX-6..IX-1
        ld      hl, #-6
        add     hl, sp
        ld      sp, hl

        ld      a, c
        ld      -6(ix), a       ; stash index (needed for both pages)
        ld      a, 5(ix)
        ld      -4(ix), a       ; buf[0] = B (4th arg)
        ld      a, 4(ix)
        ld      -3(ix), a       ; buf[1] = G (3rd arg)
        ld      a, b
        ld      -2(ix), a       ; buf[2] = R (2nd arg)
        xor     a
        ld      -1(ix), a       ; buf[3] = 0

        ; --- BIOS PIC_SET_PAL page 0 ---
        ld      e, -6(ix)       ; E = index
        ld      d, #1
        xor     a               ; A = 0 (page 0)
        ld      b, #0xFF
        push    ix
        pop     hl
        dec     hl
        dec     hl
        dec     hl
        dec     hl              ; HL = IX-4 = &buf
        ld      c, #0xA4
        push    ix
        rst     #0x08
        pop     ix

        ; --- BIOS PIC_SET_PAL page 1 ---
        ld      e, -6(ix)       ; E = index
        ld      d, #1
        ld      a, #1           ; A = 1 (page 1)
        ld      b, #0xFF
        push    ix
        pop     hl
        dec     hl
        dec     hl
        dec     hl
        dec     hl              ; HL = IX-4 = &buf
        ld      c, #0xA4
        rst     #0x08

        ; --- free scratch, restore IX, clean 2 caller stack bytes ---
        ld      hl, #6
        add     hl, sp
        ld      sp, hl
        pop     ix
        pop     iy              ; return address
        inc     sp
        inc     sp              ; drop g, b
        jp      (iy)
    __endasm;
}
