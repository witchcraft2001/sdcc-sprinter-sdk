/*
 * 42_win0exec - nested dss_exec from a win0 program.
 *
 * A win0 parent launches a standard child (HELLO.EXE). The child runs in the
 * normal DSS layout; on exit DSS restores the parent's pages (P1/P2) and the
 * RST10 trampoline restores WIN0=P0, so the parent (and its vectors) resume.
 * The filename literal lives in WIN0 and is bounced by the dss_exec override.
 */
#include <stdio.h>
#include <sprinter.h>

void main(void) {
    i16 r;
    printf("== nested dss_exec (win0 parent -> standard child) ==\r\n");
    printf("--- exec HELLO.EXE ---\r\n");
    r = dss_exec("HELLO.EXE");
    printf("--- child returned (rc=%d) ---\r\n", r);
    printf("Back in the win0 parent. Press a key.\r\n");
    dss_waitkey();
}
