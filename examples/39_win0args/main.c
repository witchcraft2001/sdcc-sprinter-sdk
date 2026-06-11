/*
 * 39_win0args - command-line passthrough in the win0 layout.
 *
 * The loader copies the command line into P0:0x0080 and crt0_win0 points
 * __cmdline there, so dss_cmdline() works normally. printf("%s", ...) reads the
 * (WIN0-resident) text itself, so it prints transparently with no bounce.
 *
 * Run from DSS with arguments, e.g.:  WIN0ARG hello world
 */

#include <stdio.h>
#include <sprinter.h>

void main(void) {
    char *cl = dss_cmdline();

    printf("== command line ==\r\n");
    printf("dss_cmdline() -> 0x%04X : '%s'\r\n", (u16)cl, cl);
    printf("Press a key.\r\n");

    dss_waitkey();
}
