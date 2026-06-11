/*
 * 31_win0spike - Phase 0 proof for the extended WIN0..WIN2 layout
 *                (see docs/ru/11_extended_layout.md).
 *
 * Goal: prove on real hardware / MAME that we can
 *   1. GETMEM a fresh RAM page P0,
 *   2. plant our own RST #10 vector in it (P0:0x0010 -> JP rst10_tramp),
 *   3. map P0 into WIN0 (evicting the DSS core page),
 *   4. issue a DSS call (PCHARS #5C) that is dispatched THROUGH our
 *      trampoline, which swaps the DSS core page back into WIN0, runs the
 *      real DSS service, then restores P0 -- and have it actually print.
 *
 * Two prints validate the pointer rule:
 *   A) string in WIN2 (this program) -> passed DIRECTLY (valid window).
 *   B) string living in WIN0 (P0, 0x0300) -> must be BOUNCED into WIN2
 *      before the call (during the call WIN0 holds the DSS page).
 *
 * This .EXE is built with CODE_LOC=0x8100 so the whole program (code, data,
 * stack, trampoline) sits in WIN2, which is never repaged -- the trampoline
 * keeps executing while it swaps WIN0 under itself.
 *
 * Border progress:  black=start  blue=P0 mapped  green=testA done
 *                    cyan=testB done  white=success  red=GETMEM failed
 *
 * Interrupts are kept DISABLED while WIN0=P0 (P0 has no real IM1 handler in
 * this spike); printing is synchronous and needs no interrupts. Proper IM1
 * trampolining is Phase 1.
 */

#include <sprinter.h>
#include <sprinter/ports.h>

/* All of these live in WIN2 (_DATA / _CODE at 0x8100+). */
static u8 dss_page;     /* DSS core page captured from WIN0 at entry  */
static u8 p0_page;      /* physical page number of our fresh P0       */
static u8 sav_win1;     /* original WIN1 page, restored after setup   */
static u8 blk;          /* GETMEM handle (for SETWIN / FREEMEM)       */
static u8 stage[64];    /* WIN2 bounce buffer                         */

static const char win2_msg[] = "WIN2 string -> DIRECT  : OK\r\n";
static const char win0_src[] = "WIN0 string -> BOUNCED : OK\r\n";

/*
 * RST #10 trampoline. Reached only via P0:0x0010 = "JP _rst10_tramp" while
 * WIN0 = P0. Swaps the DSS core page into WIN0, performs the real DSS RST,
 * restores P0. Lives in WIN2, so it survives the WIN0 swap it performs.
 */
static void rst10_tramp(void) __naked {
__asm
        di
        push    af
        xor     a
        out     (#0x3C), a          ; ROM overlay OFF (ensure RAM page in WIN0)
        ld      a, (_dss_page)
        out     (#0x82), a          ; WIN0 = DSS core page -> 0x0010 = real dispatcher
        pop     af
        rst     #0x10               ; real DSS service (C=selector, HL=ptr in WIN2)
        di                          ; DSS may have re-enabled INTs; re-assert
        push    af
        ld      a, (_p0_page)
        out     (#0x82), a          ; WIN0 = P0 again
        pop     af
        ret
__endasm;
}

void main(void) {
__asm
        ;; --- capture current windows (WIN0 = DSS core page) ---
        in      a, (#0x82)
        ld      (_dss_page), a
        in      a, (#0xA2)
        ld      (_sav_win1), a

        ;; --- GETMEM 1 page (DSS #3D): A = handle, CF = error ---
        ld      b, #1
        ld      c, #0x3D
        rst     #0x10
        jp      c, 4$               ; out of memory
        ld      (_blk), a

        ;; --- map that page into WIN1 to populate it (SETWIN1 = #39) ---
        ld      c, #0x39
        ld      b, #0
        rst     #0x10               ; A still = handle
        in      a, (#0xA2)
        ld      (_p0_page), a       ; physical page now visible in WIN1

        ;; --- plant "JP _rst10_tramp" at P0:0x0010 (== 0x4010 via WIN1) ---
        ld      hl, #0x4010
        ld      (hl), #0xC3         ; JP opcode
        inc     hl
        ld      de, #_rst10_tramp
        ld      a, e
        ld      (hl), a
        inc     hl
        ld      a, d
        ld      (hl), a

        ;; --- copy win0_src (WIN2) into P0:0x0300 (== 0x4300 via WIN1) ---
        ld      hl, #_win0_src
        ld      de, #0x4300
1$:
        ld      a, (hl)
        ld      (de), a
        inc     hl
        inc     de
        or      a
        jr      nz, 1$

        ;; --- restore WIN1, then map P0 into WIN0 (interrupts off) ---
        ld      a, (_sav_win1)
        out     (#0xA2), a
        di
        xor     a
        out     (#0x3C), a          ; ROM overlay OFF
        ld      a, (_p0_page)
        out     (#0x82), a          ; WIN0 = P0
        ld      a, #1
        out     (#0xFE), a          ; border BLUE: P0 mapped into WIN0

        ;; --- Test A: WIN2 string, passed DIRECTLY ---
        ld      hl, #_win2_msg
        ld      c, #0x5C            ; PCHARS
        rst     #0x10               ; -> P0:0x0010 -> _rst10_tramp -> DSS
        ld      a, #4
        out     (#0xFE), a          ; border GREEN

        ;; --- Test B: WIN0 string (P0:0x0300), BOUNCED into WIN2 first ---
        ld      hl, #0x0300         ; visible now (WIN0 = P0)
        ld      de, #_stage
2$:
        ld      a, (hl)
        ld      (de), a
        inc     hl
        inc     de
        or      a
        jr      nz, 2$
        ld      hl, #_stage         ; WIN2 pointer
        ld      c, #0x5C
        rst     #0x10
        ld      a, #5
        out     (#0xFE), a          ; border CYAN

        ;; --- restore WIN0 = DSS core page, free P0, re-enable INTs ---
        ld      a, (_dss_page)
        out     (#0x82), a
        ld      a, (_blk)
        ld      c, #0x3E            ; FREEMEM
        rst     #0x10
        ei
        ld      a, #7
        out     (#0xFE), a          ; border WHITE: success
        jr      5$

4$:
        ld      a, #2
        out     (#0xFE), a          ; border RED: GETMEM failed
        ei
5$:
__endasm;

    dss_waitkey();
}
