/*
 * 30_paltest - palette address discovery test.
 *
 * Per MAME emulator source (mame/src/mame/sinclair/sprinter.cpp:1240-1308):
 *
 *   - When VRAM page 0x50 is mapped into any CPU window, that window
 *     reads/writes VRAM byte at index = PORT_Y*1024 + (CPU_addr & 0x3FF).
 *     So WIN0+$03E0 == WIN1+$43E0 == WIN3+$C3E0 — only the low 10
 *     bits of the CPU address matter.
 *
 *   - VRAM bytes at offset 0x3E0..0x3FF in each 1024-byte row decode
 *     to PALETTE registers. There are 8 palette pages, each 4 bytes
 *     wide:
 *         page 0 base = $..3E0  (page 0 is the active display page)
 *         page 1 base = $..3E4
 *         page 2 base = $..3E8
 *         ...
 *         page 7 base = $..3FC
 *     Byte order: [0]=R, [1]=G, [2]=B, [3]=Y (unused).
 *
 *   - PORT_Y (#89) selects the colour index 0..255 within a palette
 *     page.
 *
 * This test verifies the above on real hardware. Screen is filled
 * with palette index 1 = bright RED via bios_setpal (known good path).
 * Then on each keypress we attempt to set palette[1] = pure BLUE
 * (R=0, G=0, B=max) via a direct CPU write through different windows.
 *
 *  Border BLACK   - reset (palette[1] = red)
 *  Border WHITE   - Test 1 ran: WIN3 + $C3E0
 *  Border BLUE    - Test 2 ran: WIN0 + $03E0
 *  Border GREEN   - Test 3 ran: WIN3 + $C3E4 (palette page 1 base)
 *  Border YELLOW  - Test 4 ran: WIN0 + $03E4
 *  Border MAGENTA - Test 5 ran: WIN1 + $43E0 via stack trampoline
 *
 * Expected (per MAME): all five tests should turn the screen BLUE.
 */

#include <sprinter.h>
#include <sprinter/gfx.h>
#include <sprinter/ports.h>

#define BORDER(c)   outp(PORT_KEYB, (c) & 7)

static void fill_screen_color1(void) {
    u16 x;
    u8  y;
    for (y = 0; y < 255; y++) {
        for (x = 0; x < SCREEN_W_320; x++) bios_putpixel(x, y, 1);
    }
    for (x = 0; x < SCREEN_W_320; x++) bios_putpixel(x, 255, 1);
    video_safe_porty();
}

static void reset_palette_red(void) {
    bios_setpal(1, 255, 0, 0);   /* full 8-bit red */
}

/* Test 1: WIN3 + $C3E0 (palette page 0).
 * Writes R=0, G=0, B=0xFF -> palette[1] should become BLUE. */
static void test_win3_palette(void) {
    __asm
        di
        in      a, (#0xE2)
        push    af
        ld      a, #0x50
        out     (#0xE2), a              ; WIN3 = VRAM #50
        ld      a, #1
        out     (#0x89), a              ; PORT_Y = 1
        xor     a
        ld      (#0xC3E0), a            ; R = 0
        ld      (#0xC3E1), a            ; G = 0
        ld      a, #0xFF
        ld      (#0xC3E2), a            ; B = max -> blue
        ld      a, #0xC0
        out     (#0x89), a
        pop     af
        out     (#0xE2), a
        ei
    __endasm;
}

/* Test 2: WIN0 + $03E0 (palette page 0). */
static void test_win0_palette(void) {
    __asm
        di
        in      a, (#0x82)
        push    af
        ld      a, #0x50
        out     (#0x82), a              ; WIN0 = VRAM #50
        ld      a, #1
        out     (#0x89), a
        xor     a
        ld      (#0x03E0), a            ; R = 0
        ld      (#0x03E1), a            ; G = 0
        ld      a, #0xFF
        ld      (#0x03E2), a            ; B = max
        ld      a, #0xC0
        out     (#0x89), a
        pop     af
        out     (#0x82), a
        ei
    __endasm;
}

/* Test 3: WIN3 + $C3E4 (palette PAGE 1 base). */
static void test_win3_palette_alt(void) {
    __asm
        di
        in      a, (#0xE2)
        push    af
        ld      a, #0x50
        out     (#0xE2), a
        ld      a, #1
        out     (#0x89), a
        xor     a
        ld      (#0xC3E4), a            ; R = 0
        ld      (#0xC3E5), a            ; G = 0
        ld      a, #0xFF
        ld      (#0xC3E6), a            ; B = max
        ld      a, #0xC0
        out     (#0x89), a
        pop     af
        out     (#0xE2), a
        ei
    __endasm;
}

/* Test 4: WIN0 + $03E4 (palette PAGE 1 base). */
static void test_win0_palette_alt(void) {
    __asm
        di
        in      a, (#0x82)
        push    af
        ld      a, #0x50
        out     (#0x82), a
        ld      a, #1
        out     (#0x89), a
        xor     a
        ld      (#0x03E4), a            ; R = 0
        ld      (#0x03E5), a            ; G = 0
        ld      a, #0xFF
        ld      (#0x03E6), a            ; B = max
        ld      a, #0xC0
        out     (#0x89), a
        pop     af
        out     (#0x82), a
        ei
    __endasm;
}

/* Test 5: WIN1 + $43E0 via stack trampoline. */
static void test_win1_palette_via_trampoline(void) {
    __asm
        push    ix
        push    iy

        ld      hl, #-32
        add     hl, sp
        ld      sp, hl
        push    hl                      ; save tramp dest

        ld      hl, #t5_template
        pop     de
        push    de
        ld      bc, #(t5_template_end - t5_template)
        ldir

        pop     hl                      ; HL = tramp dest
        push    hl
        ld      de, #t5_back
        push    de
        jp      (hl)

t5_back:
        pop     hl                      ; pop tramp dest
        ld      hl, #32
        add     hl, sp
        ld      sp, hl
        pop     iy
        pop     ix
        jr      t5_done

t5_template:
        in      a, (#0xA2)
        push    af
        di
        ld      a, #0x50
        out     (#0xA2), a              ; WIN1 = VRAM #50
        ld      a, #1
        out     (#0x89), a              ; PORT_Y = 1
        xor     a
        ld      (#0x43E0), a            ; R = 0
        ld      (#0x43E1), a            ; G = 0
        ld      a, #0xFF
        ld      (#0x43E2), a            ; B = max -> blue
        ld      a, #0xC0
        out     (#0x89), a
        pop     af
        out     (#0xA2), a              ; restore WIN1
        ei
        ret
t5_template_end:

t5_done:
    __endasm;
}

static u8 wait_or_quit(void) { return dss_waitkey(); }

void main(void) {
    u8 ch;

    video_setmode(VMODE_320);
    reset_palette_red();
    fill_screen_color1();
    BORDER(0);

    ch = wait_or_quit(); if (ch == 27) goto done;

    test_win3_palette();           BORDER(7);
    ch = wait_or_quit(); if (ch == 27) goto done;
    reset_palette_red();           BORDER(0);
    ch = wait_or_quit(); if (ch == 27) goto done;

    test_win0_palette();           BORDER(1);
    ch = wait_or_quit(); if (ch == 27) goto done;
    reset_palette_red();           BORDER(0);
    ch = wait_or_quit(); if (ch == 27) goto done;

    test_win3_palette_alt();       BORDER(4);
    ch = wait_or_quit(); if (ch == 27) goto done;
    reset_palette_red();           BORDER(0);
    ch = wait_or_quit(); if (ch == 27) goto done;

    test_win0_palette_alt();       BORDER(6);
    ch = wait_or_quit(); if (ch == 27) goto done;
    reset_palette_red();           BORDER(0);
    ch = wait_or_quit(); if (ch == 27) goto done;

    test_win1_palette_via_trampoline(); BORDER(3);
    wait_or_quit();

done:
    BORDER(0);
    video_setmode(VMODE_TEXT80);
}
