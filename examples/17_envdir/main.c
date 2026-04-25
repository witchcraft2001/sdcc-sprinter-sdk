/**
 * 17_envdir -- ENVIRON + directory API demo
 */

#include <stdio.h>
#include <stdlib.h>
#include <dir.h>
#include <sprinter.h>

void main(void) {
    char cwd[128];
    char *value;
    int rc;
    const char *tmpdir = "SDKTMP";

    dss_clrscr();
    printf("=== ENV / DIR demo ===\r\n\r\n");

    if (getcwd(cwd, sizeof(cwd))) {
        printf("Start dir: %s\r\n", cwd);
    } else {
        printf("Start dir: <error>\r\n");
    }

    rc = putenv("SDK_DEMO=");
    printf("putenv(\"SDK_DEMO=\"): %d\r\n", rc);

    rc = putenv("SDK_DEMO=ZXSPRINTER");
    printf("putenv(\"SDK_DEMO=ZXSPRINTER\"): %d\r\n", rc);

    value = getenv("sdk_demo");
    printf("getenv(\"sdk_demo\"): %s\r\n", value ? value : "(not found)");

    rmdir(tmpdir);
    rc = mkdir(tmpdir);
    printf("mkdir(\"%s\"): %d\r\n", tmpdir, rc);

    rc = chdir(tmpdir);
    printf("chdir(\"%s\"): %d\r\n", tmpdir, rc);
    if (rc == 0 && getcwd(cwd, sizeof(cwd))) {
        printf("Inside dir: %s\r\n", cwd);
    }

    rc = chdir("..");
    printf("chdir(\"..\"): %d\r\n", rc);
    if (rc == 0 && getcwd(cwd, sizeof(cwd))) {
        printf("Back dir  : %s\r\n", cwd);
    }

    rc = rmdir(tmpdir);
    printf("rmdir(\"%s\"): %d\r\n", tmpdir, rc);

    rc = putenv("SDK_DEMO=");
    printf("delete SDK_DEMO: %d\r\n", rc);
    value = getenv("SDK_DEMO");
    printf("getenv(\"SDK_DEMO\"): %s\r\n", value ? value : "(still present)");

    printf("\r\nPress any key...\r\n");
    getchar();
}
