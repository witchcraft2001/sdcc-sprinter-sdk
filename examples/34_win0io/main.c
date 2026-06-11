/*
 * 34_win0io - Phase 3 demo: pointer marshalling for the WIN0..WIN2 layout.
 *
 * Runs from 0x0300 inside WIN0 (same loader/crt0 as 33_win0hello). It exercises
 * the bounce wrappers (win0_io.c) on pointers that genuinely live in WIN0:
 *   [in ] an inline string literal (in _CODE/WIN0) printed via w_puts,
 *   [out] DSS writes the cwd into a WIN0 buffer (0x0100) via w_curdir,
 *   [file] a file is created/written from WIN0 literals and read back into a
 *          WIN0 buffer (w_creat/w_write/w_open/w_read).
 *
 * The literal address is shown in hex to prove it is < 0x4000 (WIN0). With the
 * plain dss_* wrappers these pointers would be invisible to DSS during the
 * call; the bounce makes them work.
 */

#include <sprinter.h>
#include "win0_io.h"

void main(void) {
    const char *lit = "inline literal stored in WIN0";
    char *win0buf = (char *)0x0100;   /* free WIN0 RAM, below code @0x0300 */
    i16 fd, n;

    w_puts("== Phase 3: pointer marshalling (bounce) ==\r\n");

    /* [in] string literal that lives in WIN0 (_CODE) */
    w_puts("[in ] literal at 0x");
    w_puthex((u16)lit);
    w_puts(" -> '");
    w_puts(lit);
    w_puts("'\r\n");

    /* [out] DSS writes cwd into a WIN0 buffer; copied back by w_curdir */
    w_curdir(win0buf);
    w_puts("[out] cwd into WIN0 0x0100 -> '");
    w_puts(win0buf);
    w_puts("'\r\n");

    /* [file] write via WIN0 literals, read back into a WIN0 buffer */
    fd = w_creat("P3TEST.TXT");
    if (fd >= 0) {
        w_write((u8)fd, "bounced file data", 17);
        dss_close((u8)fd);
        fd = w_open("P3TEST.TXT", O_RDONLY);
        if (fd >= 0) {
            n = w_read((u8)fd, win0buf, 60);
            dss_close((u8)fd);
            if (n < 0) n = 0;
            win0buf[n] = 0;
            w_puts("[file] read back -> '");
            w_puts(win0buf);
            w_puts("'\r\n");
        } else {
            w_puts("[file] reopen failed\r\n");
        }
    } else {
        w_puts("[file] create failed (read-only disk?)\r\n");
    }

    w_puts("Done. Press a key.\r\n");
    dss_waitkey();
}
