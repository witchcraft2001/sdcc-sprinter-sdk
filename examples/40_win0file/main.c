/*
 * 40_win0file - full transparency proof: stdio in the win0 layout, unchanged.
 *
 * Uses plain stdio (fopen/fputs/fgets) with a string-literal filename and
 * literal data -- all of which live in WIN0. The win0 dss_* override makes
 * fopen()/fwrite() bounce the WIN0 pointers automatically, so this ordinary C
 * program works with NO source changes and the normal layout pays no overhead.
 */

#include <stdio.h>
#include <sprinter.h>

void main(void) {
    FILE *f;
    char buf[64];

    printf("== stdio over win0 (literal filename + data) ==\r\n");

    f = fopen("P4TEST.TXT", "w");
    if (f) {
        fputs("stdio via win0 override OK", f);
        fclose(f);
    } else {
        printf("create failed (read-only disk?)\r\n");
    }

    f = fopen("P4TEST.TXT", "r");
    if (f) {
        buf[0] = 0;
        fgets(buf, sizeof(buf), f);
        fclose(f);
        printf("read back: '%s'\r\n", buf);
    } else {
        printf("reopen failed\r\n");
    }

    printf("Press a key.\r\n");
    dss_waitkey();
}
