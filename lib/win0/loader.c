/*
 * loader.c - stage-1 PRELOAD loader for the extended WIN0..WIN2 layout (SDK).
 *
 * Built as a PRELOAD .EXE by win0_exe.py: DSS loads only this loader to 0x8100,
 * leaves the .EXE file open positioned right after it, and jumps (handle at the
 * PSP, IX-3 == 0x807D). The loader then streams the payload from the file:
 *   - read the 6-byte size table {win0_len, win1_len, win2_len},
 *   - GETMEM P0 and DSS.Read the WIN0 code blob into P0:0x0180,
 *   - GETMEM P1 (if any) and read the WIN1 code blob into P1:0x4000,
 *   - GETMEM P2 and read the WIN2 trampolines into P2:0x8000,
 * then write boot params at P0:0x0040 and jump to 0x0180 (crt0_win0).
 *
 * Multi-page: code/rodata may span WIN0 (0x0180-0x3FFF) + WIN1 (0x4000-0x7FFF);
 * data/stack/trampolines live in the private WIN2 page P2. DSS.Exit frees
 * P0/P1/P2 (allocated by this process) on return.
 */

#include <sprinter.h>
#include <sprinter/ports.h>
#include <string.h>

static u8 g_dss, g_p0, g_p1, g_p2;
static u8 g_b0, g_b1, g_b2;
static u16 tbl[3];                       /* win0_len, win1_len, win2_len */

void main(void) {
    u8 handle = *(u8 *)0x807D;           /* EXE file handle from PSP (IX-3) */

    g_dss = inp(PORT_WIN0);
    dss_read(handle, tbl, 6);

    /* WIN0 code -> P0:0x0180 (== 0x4180 via WIN1) */
    g_b0 = dss_getmem();
    dss_setwin(1, g_b0);
    g_p0 = inp(PORT_WIN1);
    dss_read(handle, (void *)0x4180, tbl[0]);

    /* WIN1 code -> P1:0x4000 (== 0x4000 via WIN1), if present */
    if (tbl[1]) {
        g_b1 = dss_getmem();
        dss_setwin(1, g_b1);
        g_p1 = inp(PORT_WIN1);
        dss_read(handle, (void *)0x4000, tbl[1]);
    } else {
        g_p1 = 0;
    }

    /* WIN2 trampolines -> P2:0x8000 (== 0x4000 via WIN1) */
    g_b2 = dss_getmem();
    dss_setwin(1, g_b2);
    g_p2 = inp(PORT_WIN1);
    dss_read(handle, (void *)0x4000, tbl[2]);

    dss_close(handle);

    /* boot params at P0:0x0040 + command line at P0:0x0080 (WIN1 = P0) */
    dss_setwin(1, g_b0);
    ((u8 *)0x4040)[0] = g_dss;
    ((u8 *)0x4040)[1] = g_p0;
    ((u8 *)0x4040)[2] = g_p1;
    ((u8 *)0x4040)[3] = g_p2;
    {
        /* Copy the whole PSP cmd-line region {len,text} (0x8080-0x80FF) to
         * P0:0x0080, then force a NUL after the text so dss_cmdline() (returns
         * 0x0081) yields a proper C string. DSS terminates with \0/\r/\n. */
        u8 *d = (u8 *)0x4080;            /* P0:0x0080 via WIN1 */
        u16 i;
        memcpy(d, (void *)0x8080, 128);
        i = 1;
        while (i < 127 && d[i] && d[i] != '\r' && d[i] != '\n') i++;
        d[i] = 0;
    }

    /* map P0 into WIN0, P1 into WIN1, jump; crt0_win0 sets WIN2=P2 + SP */
    __asm
        di
        xor     a
        out     (#0x3C), a          ; ROM overlay off
        ld      a, (_g_p0)
        out     (#0x82), a          ; WIN0 = P0
        ld      a, (_g_p1)
        out     (#0xA2), a          ; WIN1 = P1
        ld      hl, #0x0180
        jp      (hl)                ; crt0_win0 entry (= WIN0_CODE origin)
    __endasm;
}
