/*
 * 35_win0app - win0 build profile + private full WIN2.
 *
 * Ordinary C (printf), built in the extended layout with a one-line Makefile.
 * The 8 KB global lives in the private WIN2 page; filling it proves the program
 * owns the full WIN2 window.
 */

#include <stdio.h>
#include <sprinter.h>

static u8 bigbuf[8192];          /* in WIN2 (private P2) */

void main(void) {
    u16 i;

    printf("== win0 build profile + private WIN2 ==\r\n");
    printf("Makefile is one line: include ../win0.mk\r\n");

    for (i = 0; i < sizeof(bigbuf); i++) bigbuf[i] = (u8)i;

    printf("8 KB global at 0x%04X filled OK -> full private WIN2.\r\n", (u16)bigbuf);
    printf("Code in WIN0, DSS via trampolines, IM1 alive. Press a key.\r\n");

    dss_waitkey();
}
