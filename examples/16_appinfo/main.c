/**
 * 16_appinfo -- APPINFO / EX_PATH / getcwd / fnsplit demo
 */

#include <string.h>
#include <dir.h>
#include <sprinter.h>

static char params[128];
static char app_dir[128];
static char full_path[128];
static char cwd[128];
static char drive[16];
static char path[128];
static char fname[16];
static char ext[8];
static char direct_name[16];

static void print_hex16(u16 value) {
    const char hex[] = "0123456789ABCDEF";
    dss_puts("0x");
    dss_putchar(hex[(value >> 12) & 0x0F]);
    dss_putchar(hex[(value >> 8) & 0x0F]);
    dss_putchar(hex[(value >> 4) & 0x0F]);
    dss_putchar(hex[value & 0x0F]);
}

static void print_label_value(const char *label, const char *value) {
    dss_puts(label);
    dss_puts(": ");
    dss_puts(value && value[0] ? value : "(empty)");
    dss_puts("\r\n");
}

static void show_appinfo(u8 subfunc, const char *label, char *buf, u16 size) {
    memset(buf, 0, size);
    if (dss_appinfo(subfunc, buf) == 0)
        print_label_value(label, buf);
    else
        print_label_value(label, "<error>");
}

void main(void) {
    int flags;

    dss_clrscr();
    dss_puts("=== APPINFO / PATH demo ===\r\n\r\n");

    show_appinfo(APPINFO_PARAMS, "APPINFO params", params, sizeof(params));
    show_appinfo(APPINFO_DIR, "APPINFO dir", app_dir, sizeof(app_dir));
    show_appinfo(APPINFO_FULL, "APPINFO full", full_path, sizeof(full_path));

    memset(cwd, 0, sizeof(cwd));
    if (getcwd(cwd, sizeof(cwd))) {
        print_label_value("getcwd()", cwd);
    } else {
        print_label_value("getcwd()", "<error>");
    }

    if (full_path[0]) {
        memset(drive, 0, sizeof(drive));
        memset(path, 0, sizeof(path));
        memset(fname, 0, sizeof(fname));
        memset(ext, 0, sizeof(ext));
        flags = fnsplit(full_path, drive, path, fname, ext);
        dss_puts("\r\nfnsplit flags: ");
        print_hex16((u16)flags);
        dss_puts("\r\n");
        print_label_value("drive", drive);
        print_label_value("path ", path);
        print_label_value("name ", fname);
        print_label_value("ext  ", ext);

        memset(direct_name, 0, sizeof(direct_name));
        if (dss_expath(full_path, direct_name, EXPATH_NAME) == 0) {
            print_label_value("EXPATH name", direct_name);
        } else {
            print_label_value("EXPATH name", "<error>");
        }
    }

    dss_puts("\r\nPress any key...\r\n");
    dss_waitkey();
}
