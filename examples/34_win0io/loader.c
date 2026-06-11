/*
 * loader.c - stage-1 loader for the extended WIN0..WIN2 layout (Phase 2).
 *
 * A normal DSS .EXE (loads at 0x8100, WIN2). It:
 *   1. captures the DSS core page (WIN0) and its own page (WIN2 = shared P2),
 *   2. GETMEMs a fresh page P0 for WIN0 code,
 *   3. copies the payload WIN0 blob into P0:0x0300 (via WIN1),
 *   4. writes boot params at P0:0x0040,
 *   5. copies the payload WIN2 blob (trampolines) into 0xA000 of the shared
 *      page,
 *   6. maps P0 into WIN0, sets SP, and jumps to the payload at 0x0300.
 *
 * The payload then runs ordinary C from inside WIN0 (see hello.c, crt0_win0).
 */

#include <sprinter.h>
#include <sprinter/ports.h>
#include <string.h>
#include "payload_blobs.h"

static u8 g_dss, g_p0, g_p2, g_blk;

void main(void) {
    g_dss = inp(PORT_WIN0);                 /* DSS core page (WIN0)         */
    g_p2  = inp(PORT_WIN2);                  /* our own page (shared P2)     */
    g_blk = dss_getmem();                    /* allocate P0                  */
    dss_setwin(1, g_blk);                    /* map P0 into WIN1 to fill it  */
    g_p0  = inp(PORT_WIN1);                  /* physical page of P0          */

    /* payload WIN0 code -> P0:0x0300 (== 0x4000 + 0x0300 via WIN1) */
    memcpy((void *)(0x4000 + PAYLOAD_WIN0_ADDR), payload_win0, PAYLOAD_WIN0_LEN);

    /* boot params at P0:0x0040 (== 0x4040 via WIN1) */
    ((u8 *)0x4040)[0] = g_dss;
    ((u8 *)0x4040)[1] = g_p0;
    ((u8 *)0x4040)[2] = 0;                   /* p1 unused for single-page */
    ((u8 *)0x4040)[3] = g_p2;

    /* payload WIN2 trampolines -> 0xA000 of the shared page (WIN2) */
    memcpy((void *)PAYLOAD_WIN2_ADDR, payload_win2, PAYLOAD_WIN2_LEN);

    /* map P0 into WIN0, set payload stack, jump to 0x0300 */
    __asm
        di
        xor     a
        out     (#0x3C), a          ; ROM overlay off
        ld      a, (_g_p0)
        out     (#0x82), a          ; WIN0 = P0
        ld      sp, #0xBF00
        ld      hl, #0x0300
        jp      (hl)
    __endasm;
}
