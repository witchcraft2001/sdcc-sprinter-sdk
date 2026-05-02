#include <stdio.h>
#include <sprinter/dss.h>

void main(void) {
    char *cmdline = dss_cmdline();

    printf("=== CHILD ===\n");
    printf("cmdline: %s\n", cmdline);
    dss_exit(42);
}
