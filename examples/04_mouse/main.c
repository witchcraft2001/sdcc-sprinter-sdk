/**
 * 04_mouse — Mouse input example for ZX Sprinter
 *
 * Demonstrates mouse driver: initialization, cursor, position tracking.
 * Press ESC or click right button to exit.
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

void main(void) {
    u8 buttons;
    mouse_state_t ms;

    dss_clrscr();
    dss_puts("=== Mouse Example ===\r\n\r\n");

    buttons = mouse_init();
    if (!buttons) {
        dss_puts("No mouse detected!\r\n");
        dss_puts("Press any key to exit.\r\n");
        dss_waitkey();
        return;
    }

    dss_puts("Mouse found!\r\n\r\n");

    /* Set boundaries */
    mouse_xbound(0, 319);
    mouse_ybound(0, 255);

    mouse_show();

    dss_puts("Move mouse. Right-click or press key to exit.\r\n\r\n");

    while (1) {
        /* Check keyboard */
        if (dss_kbhit()) {
            break;
        }

        /* Read mouse state */
        mouse_stat(&ms);

        /* Show position */
        dss_gotoxy(1, 8);
        dss_puts("X: ");
        print_num(ms.x);
        dss_puts("   \r\n");
        dss_puts("Y: ");
        print_num(ms.y);
        dss_puts("   \r\n");
        dss_puts("Buttons: ");
        if (ms.buttons & MS_BTN_LEFT)   dss_puts("L");
        if (ms.buttons & MS_BTN_RIGHT)  dss_puts("R");
        if (ms.buttons & MS_BTN_MIDDLE) dss_puts("M");
        dss_puts("   \r\n");

        /* Exit on right-click */
        if (ms.buttons & MS_BTN_RIGHT) {
            break;
        }
    }

    mouse_hide();
    dss_puts("\r\nDone!\r\n");
}
