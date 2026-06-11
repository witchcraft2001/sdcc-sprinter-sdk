/*
 * 38_win0printf - Phase 4d: a perfectly ordinary printf program built in the
 * extended WIN0..WIN2 layout with NO source changes.
 *
 * printf parses the format string itself (in WIN0) and emits characters by
 * VALUE through dss_putchar -- it never hands a pointer to DSS -- so it is
 * transparently correct in the win0 layout. Build is one line:
 *     APP=win0pf; SRCS=main.c; include ../win0.mk
 */

#include <stdio.h>
#include <sprinter.h>

void main(void) {
    int i;

    printf("Phase 4d: printf in the win0 layout (no source changes)\r\n");
    for (i = 1; i <= 5; i++)
        printf("  line %d : value 0x%04X\r\n", i, i * 0x1111);
    printf("printf runs from WIN0, DSS via trampolines. Press a key.\r\n");

    dss_waitkey();
}
