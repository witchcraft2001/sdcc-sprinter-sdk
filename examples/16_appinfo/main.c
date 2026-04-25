/**
 * 16_appinfo -- APPINFO / EX_PATH / getcwd / fnsplit demo
 */

#include <stdio.h>
#include <dir.h>
#include <sprinter.h>

static void show_appinfo(u8 subfunc, const char *label, char *buf) {
    buf[0] = 0;
    if (dss_appinfo(subfunc, buf) == 0) {
        printf("%s: %s\n", label, buf[0] ? buf : "(empty)");
    } else {
        printf("%s: <error>\n", label);
    }
}

void main(void) {
    char params[128];
    char app_dir[128];
    char full_path[128];
    char cwd[128];
    char drive[16];
    char path[128];
    char fname[16];
    char ext[8];
    char direct_name[16];
    int flags;

    dss_clrscr();
    printf("=== APPINFO / PATH demo ===\n\n");

    show_appinfo(APPINFO_PARAMS, "APPINFO params", params);
    show_appinfo(APPINFO_DIR, "APPINFO dir", app_dir);
    show_appinfo(APPINFO_FULL, "APPINFO full", full_path);

    if (getcwd(cwd, sizeof(cwd))) {
        printf("getcwd(): %s\n", cwd);
    } else {
        printf("getcwd(): <error>\n");
    }

    if (full_path[0]) {
        flags = fnsplit(full_path, drive, path, fname, ext);
        printf("\nfnsplit flags: 0x%X\n", (u16)flags);
        printf("drive: %s\n", drive[0] ? drive : "(none)");
        printf("path : %s\n", path[0] ? path : "(none)");
        printf("name : %s\n", fname[0] ? fname : "(none)");
        printf("ext  : %s\n", ext[0] ? ext : "(none)");

        direct_name[0] = 0;
        if (dss_expath(full_path, direct_name, EXPATH_NAME) == 0) {
            printf("EXPATH name: %s\n", direct_name[0] ? direct_name : "(empty)");
        } else {
            printf("EXPATH name: <error>\n");
        }
    }

    printf("\nPress any key...\n");
    getchar();
}
