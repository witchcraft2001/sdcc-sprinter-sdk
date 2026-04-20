/**
 * 05_keyinput — Keyboard input and system info example
 *
 * Shows BIOS version, system date/time, and keyboard echo loop.
 * Press ESC to exit.
 */

#include <sprinter.h>

static void print_num(u16 n) {
    char buf[6];
    u8 i = 5;
    buf[5] = 0;
    if (n == 0) {
        dss_putchar('0');
        return;
    }
    while (n > 0) {
        i--;
        buf[i] = '0' + (n % 10);
        n /= 10;
    }
    dss_puts(&buf[i]);
}

static void print_hex8(u8 val) {
    const char hex[] = "0123456789ABCDEF";
    dss_putchar(hex[(val >> 4) & 0x0F]);
    dss_putchar(hex[val & 0x0F]);
}

void main(void) {
    dss_date_t date;
    dss_time_t time;
    dss_key_t key;
    u16 ver;
    u8 disk;

    dss_clrscr();
    dss_puts("=== ZX Sprinter System Info ===\r\n\r\n");

    /* BIOS version */
    ver = bios_version();
    dss_puts("BIOS version: ");
    print_hex8((u8)(ver >> 8));
    dss_putchar('.');
    print_hex8((u8)(ver & 0xFF));
    dss_puts("\r\n");

    /* Board ID */
    dss_puts("Board ID: ");
    print_hex8(bios_board_id());
    dss_puts("\r\n");

    /* Current disk */
    disk = dss_getdisk();
    dss_puts("Current disk: ");
    dss_putchar('A' + disk);
    dss_puts(":\r\n");

    /* Date and time */
    dss_getdate(&date);
    dss_gettime(&time);

    dss_puts("Date: ");
    print_num(date.year);
    dss_putchar('-');
    print_num(date.month);
    dss_putchar('-');
    print_num(date.day);
    dss_puts("\r\n");

    dss_puts("Time: ");
    print_num(time.hour);
    dss_putchar(':');
    print_num(time.minute);
    dss_putchar(':');
    print_num(time.second);
    dss_puts("\r\n");

    /* Keyboard echo loop */
    dss_puts("\r\n--- Keyboard Echo ---\r\n");
    dss_puts("Type characters (ESC to quit, Ctrl+X/Ctrl+Z shown with modifiers):\r\n\r\n");

    while (1) {
        dss_waitkey_ex(&key);

        if (key.ascii == 27) {  /* ESC */
            break;
        }

        if ((key.modifiers & DSS_KEYMOD_CTRL) &&
            (key.ascii == 'X' || key.ascii == 'x' ||
             key.ascii == 'Z' || key.ascii == 'z')) {
            dss_puts("[CTRL+");
            dss_putchar((key.ascii >= 'a' && key.ascii <= 'z') ? (key.ascii - 32) : key.ascii);
            dss_puts(" mods=0x");
            print_hex8(key.modifiers);
            dss_putchar(']');
        } else if (key.ascii == 13) {  /* Enter */
            dss_puts("\r\n");
        } else if (key.ascii >= 32 && key.ascii < 127) {
            dss_putchar(key.ascii);
        } else {
            /* Show hex code for special keys */
            dss_puts("[0x");
            print_hex8(key.ascii);
            dss_puts(" mods=0x");
            print_hex8(key.modifiers);
            dss_putchar(' ');
            dss_puts("scan=0x");
            print_hex8(key.scan);
            dss_putchar(']');
        }
    }

    dss_puts("\r\n\r\nGoodbye!\r\n");
}
