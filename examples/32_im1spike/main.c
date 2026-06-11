/*
 * 32_im1spike - Phase 1 proof: IM1 trampoline for the extended WIN0..WIN2
 *               layout (see docs/ru/11_extended_layout.md, 11.9).
 *
 * Phase 0 (31_win0spike) proved RST #10 trampolining + the pointer bounce.
 * Phase 1 proves the hard part: running our C-level code with WIN0 = P0 and
 * INTERRUPTS ENABLED, while DSS's per-frame work (keyscan / cursor / mouse)
 * keeps running -- routed through our own IM1 trampoline.
 *
 * Mechanism: P0:0x0038 = "JP rst38_tramp" (in WIN2). On every frame INT:
 *   rst38_tramp: save AF/HL -> WIN0 = DSS core page -> push resume38 ->
 *                JP 0x0038 (DSS .Handler: saves all regs, KEYSCAN + mouse +
 *                cursor, EI/RETI -> resume38)
 *   resume38:    WIN0 = P0, bump _int_cnt, set border = _int_cnt&7
 *                (visible heartbeat), restore AF/HL, EI/RETI.
 *
 * Test: after setup, ENABLE interrupts with WIN0 = P0 and sit in a pure
 * busy-delay (~2 s) that touches no DSS and no border. If the border cycles
 * colours on its own during the delay, every frame INT was serviced through
 * our rst38_tramp -> DSS handler and back. Frozen border / crash = broken.
 *
 * Then FREEMEM is verified by re-GETMEM (magenta = page came back, red =
 * could not re-allocate). Finally dss_waitkey() holds the screen.
 *
 * Built CODE_LOC=0x8100 (whole program in WIN2, never repaged).
 */

#include <sprinter.h>
#include <sprinter/ports.h>

static u8 dss_page;     /* DSS core page captured from WIN0 at entry */
static u8 p0_page;      /* physical page of our fresh P0            */
static u8 sav_win1;     /* original WIN1, restored after setup      */
static u8 blk;          /* GETMEM handle                            */
static u8 int_cnt;      /* bumped by the IM1 trampoline each frame  */

static const char msg_spin[] = "P1: WIN0=P0, INT on. Border must cycle...\r\n";
static const char msg_ok[]   = "IM1 trampoline alive. Press a key.\r\n";

/* RST #10 trampoline (proved in Phase 0). Keeps DI; used only from DI code. */
static void rst10_tramp(void) __naked {
__asm
        di
        push    af
        xor     a
        out     (#0x3C), a
        ld      a, (_dss_page)
        out     (#0x82), a          ; WIN0 = DSS
        pop     af
        rst     #0x10
        di
        push    af
        ld      a, (_p0_page)
        out     (#0x82), a          ; WIN0 = P0
        pop     af
        ret
__endasm;
}

/* IM1 trampoline. Reached via P0:0x0038 = JP _rst38_tramp while WIN0 = P0. */
static void rst38_tramp(void) __naked {
__asm
        push    af
        push    hl
        ld      a, (_dss_page)
        out     (#0x82), a          ; WIN0 = DSS core page (0x0038 = .Handler)
        ld      hl, #im1_resume
        push    hl                  ; DSS .Handler RETI lands here
        ld      hl, #0x0038
        jp      (hl)                ; into DSS frame handler (keyscan/mouse/cursor)
im1_resume:
        di
        ld      a, (_p0_page)
        out     (#0x82), a          ; WIN0 = P0
        ld      a, (_int_cnt)
        inc     a
        ld      (_int_cnt), a
        and     #7
        out     (#0xFE), a          ; heartbeat border = INT count & 7
        pop     hl                  ; restore interrupted HL
        pop     af                  ; restore interrupted AF
        ei
        reti
__endasm;
}

void main(void) {
__asm
        ;; --- capture windows, GETMEM P0 (DSS #3D) ---
        in      a, (#0x82)
        ld      (_dss_page), a
        in      a, (#0xA2)
        ld      (_sav_win1), a
        ld      b, #1
        ld      c, #0x3D
        rst     #0x10
        jp      c, 9$               ; out of memory -> red
        ld      (_blk), a

        ;; --- map P0 into WIN1, plant vectors at P0:0x0010 and P0:0x0038 ---
        ld      c, #0x39            ; SETWIN1
        ld      b, #0
        rst     #0x10
        in      a, (#0xA2)
        ld      (_p0_page), a

        ld      hl, #0x4010         ; P0:0x0010
        ld      (hl), #0xC3
        inc     hl
        ld      de, #_rst10_tramp
        ld      a, e
        ld      (hl), a
        inc     hl
        ld      a, d
        ld      (hl), a

        ld      hl, #0x4038         ; P0:0x0038
        ld      (hl), #0xC3
        inc     hl
        ld      de, #_rst38_tramp
        ld      a, e
        ld      (hl), a
        inc     hl
        ld      a, d
        ld      (hl), a

        ;; --- restore WIN1, map P0 into WIN0 (DI) ---
        ld      a, (_sav_win1)
        out     (#0xA2), a
        di
        xor     a
        out     (#0x3C), a
        ld      a, (_p0_page)
        out     (#0x82), a          ; WIN0 = P0
        xor     a
        ld      (_int_cnt), a

        ;; --- prompt (DI, via RST10 trampoline) ---
        ld      hl, #_msg_spin
        ld      c, #0x5C
        rst     #0x10

        ;; --- THE TEST: enable INTs with WIN0 = P0, busy-spin ~2s ---
        ei
        ld      d, #6               ; outer
1$:
        ld      bc, #0              ; inner 65536
2$:
        dec     bc
        ld      a, b
        or      c
        jr      nz, 2$
        dec     d
        jr      nz, 1$
        di                          ; stop heartbeat

        ;; --- "alive" message (DI, via trampoline) ---
        ld      hl, #_msg_ok
        ld      c, #0x5C
        rst     #0x10

        ;; --- restore WIN0 = DSS, FREEMEM, verify by re-GETMEM ---
        ld      a, (_dss_page)
        out     (#0x82), a
        ld      a, (_blk)
        ld      c, #0x3E            ; FREEMEM
        rst     #0x10
        ld      b, #1               ; re-GETMEM to confirm the page came back
        ld      c, #0x3D
        rst     #0x10
        jr      c, 8$               ; could not re-allocate -> red
        ld      c, #0x3E            ; free it again (A = page)
        rst     #0x10
        ld      a, #3
        out     (#0xFE), a          ; magenta: FREEMEM ok
        jr      7$
8$:
        ld      a, #2
        out     (#0xFE), a          ; red: re-alloc failed
        jr      7$
9$:
        ld      a, #2
        out     (#0xFE), a          ; red: GETMEM failed
7$:
        ei
__endasm;

    dss_waitkey();
}
