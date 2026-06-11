/*
 * hello.c - the WIN0 payload: ordinary C, linked at 0x0300, running from
 * inside WIN0. All DSS access goes through the RST trampolines installed by
 * crt0_win0 / win0_rt. (Phase 2 of docs/ru/11_extended_layout.md)
 *
 * The two strings are mutable globals, so SDCC places them in _INITIALIZED
 * (WIN2, >=0x4000) and gsinit fills them at startup -- valid pointers for
 * DSS. (Inline string literals would sit in _CODE/WIN0 and need the Phase 3
 * bounce; not used here.)
 */

#include <sprinter.h>

static char msg1[] = "Hello from C running at 0x0300 inside WIN0!\r\n";
static char msg2[] = "DSS reached via RST trampolines. Press a key.\r\n";

void main(void) {
    dss_puts(msg1);
    dss_puts(msg2);
    dss_waitkey();
}
