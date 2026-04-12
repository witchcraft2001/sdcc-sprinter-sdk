/**
 * 01_hello — Hello World for ZX Sprinter
 *
 * Simplest possible program: prints a message and waits for a key.
 */

#include <sprinter.h>

void main(void) {
    dss_puts("Hello from ZX Sprinter!\r\n");
    dss_puts("SDCC SDK v1.0\r\n");
    dss_puts("\r\n");
    dss_puts("Press any key to exit...\r\n");

    dss_waitkey();
}
