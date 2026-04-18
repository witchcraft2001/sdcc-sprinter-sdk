/**
 * 15_exec -- Execute child program and report result/error
 */

#include <stdio.h>
#include <sprinter.h>

void main(void) {
    const char *cmd = "CHILD.EXE arg1 arg2";
    u8 err = 0;
    i16 rc;

    printf("=== EXEC demo ===\n\n");
    printf("Running: %s\n", cmd);

    rc = dss_exec_ex(cmd, &err);
    if (rc < 0) {
        printf("EXEC failed: DSS error A=0x%X (%u)\n", (u16)err, (u16)err);
    } else {
        printf("Child exit code: %d (0x%X)\n", rc, (u16)rc);
    }

    printf("\nPress any key...\n");
    getchar();
}
