/*
 * 41_win0misc - directory/file pointer wrappers + page-leak check (win0).
 *
 * Exercises dss_mkdir/chdir/rmdir/ffirst with WIN0 string literals (bounced by
 * the win0 dss_* overrides). dss_meminfo is already win0-safe (fills the struct
 * in the wrapper). Run TWICE: if "free pages at start" is the same both runs,
 * DSS.Exit freed the program pages (no leak).
 */

#include <stdio.h>
#include <sprinter.h>

void main(void) {
    u16 total, free1, free2;
    dss_find_t fr;

    dss_meminfo(&total, &free1);
    printf("== win0 dir ops + leak check ==\r\n");
    printf("free pages at start: %u (of %u)\r\n", free1, total);

    printf("mkdir WTMP -> %u\r\n", dss_mkdir("WTMP"));
    printf("chdir WTMP -> %u\r\n", dss_chdir("WTMP"));
    dss_chdir("..");
    printf("rmdir WTMP -> %u\r\n", dss_rmdir("WTMP"));

    if (dss_ffirst("*.*", &fr, 0) == 0)
        printf("ffirst *.* -> '%s'\r\n", fr.ff_name);
    else
        printf("ffirst: none\r\n");

    dss_meminfo(&total, &free2);
    printf("free pages now: %u (run twice; equal across runs = no exit leak)\r\n",
           free2);
    printf("Press a key.\r\n");
    dss_waitkey();
}
