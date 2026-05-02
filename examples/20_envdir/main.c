/**
 * 20_envdir -- ENVIRON + directory API demo
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
    printf("=== ENV / DIR demo ===\n\n");

    if (getcwd(cwd, sizeof(cwd))) {
        printf("Start dir: %s\n", cwd);
    } else {
        printf("Start dir: <error>\n");
    }

    rc = putenv("SDK_DEMO=");
    printf("putenv(\"SDK_DEMO=\"): %d\n", rc);

    rc = putenv("SDK_DEMO=ZXSPRINTER");
    printf("putenv(\"SDK_DEMO=ZXSPRINTER\"): %d\n", rc);

    value = getenv("sdk_demo");
    printf("getenv(\"sdk_demo\"): %s\n", value ? value : "(not found)");

    rmdir(tmpdir);
    rc = mkdir(tmpdir);
    printf("mkdir(\"%s\"): %d\n", tmpdir, rc);

    rc = chdir(tmpdir);
    printf("chdir(\"%s\"): %d\n", tmpdir, rc);
    if (rc == 0 && getcwd(cwd, sizeof(cwd))) {
        printf("Inside dir: %s\n", cwd);
    }

    rc = chdir("..");
    printf("chdir(\"..\"): %d\n", rc);
    if (rc == 0 && getcwd(cwd, sizeof(cwd))) {
        printf("Back dir  : %s\n", cwd);
    }

    rc = rmdir(tmpdir);
    printf("rmdir(\"%s\"): %d\n", tmpdir, rc);

    rc = putenv("SDK_DEMO=");
    printf("delete SDK_DEMO: %d\n", rc);
    value = getenv("SDK_DEMO");
    printf("getenv(\"SDK_DEMO\"): %s\n", value ? value : "(still present)");

    printf("\nPress any key...\n");
    getchar();
}
