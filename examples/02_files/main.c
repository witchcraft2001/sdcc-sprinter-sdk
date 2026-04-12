/**
 * 02_files — File I/O example for ZX Sprinter
 *
 * Demonstrates creating, writing, reading, and deleting files via DSS.
 */

#include <sprinter.h>

/* Simple itoa for small positive numbers */
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
    i16 fd;
    i16 bytes;
    char buf[64];
    const char *filename = "TEST.TXT";
    const char *message = "Hello from SDCC on Sprinter!\r\n";
    u8 i;

    dss_puts("=== File I/O Example ===\r\n\r\n");

    /* Create and write a file */
    dss_puts("Creating ");
    dss_puts(filename);
    dss_puts("...\r\n");

    fd = dss_creat(filename);
    if (fd < 0) {
        dss_puts("Error: cannot create file!\r\n");
        dss_waitkey();
        return;
    }

    /* Calculate string length */
    for (i = 0; message[i]; i++);

    bytes = dss_write((u8)fd, message, (u16)i);
    dss_close((u8)fd);

    dss_puts("Wrote ");
    print_num(bytes);
    dss_puts(" bytes.\r\n\r\n");

    /* Read back the file */
    dss_puts("Reading back...\r\n");

    fd = dss_open(filename, O_RDONLY);
    if (fd < 0) {
        dss_puts("Error: cannot open file!\r\n");
        dss_waitkey();
        return;
    }

    bytes = dss_read((u8)fd, buf, 63);
    dss_close((u8)fd);

    if (bytes > 0) {
        buf[bytes] = 0;
        dss_puts("Content: ");
        dss_puts(buf);
    }

    /* Clean up */
    dss_puts("\r\nDeleting test file...\r\n");
    dss_delete(filename);

    dss_puts("\r\nDone! Press any key.\r\n");
    dss_waitkey();
}
