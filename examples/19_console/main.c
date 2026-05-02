/**
 * 19_console -- DSS console window and video-state demo
 */

#include <stdio.h>
#include <sprinter.h>

static void print_line(u8 x, u8 y, const char *text) {
    dss_gotoxy(x, y);
    dss_puts(text);
}

static void print_status(u8 y, const char *text) {
    dss_gotoxy(1, y);
    dss_puts("                                        ");
    dss_gotoxy(1, y);
    dss_puts(text);
}

static void wait_fresh_key(void) __naked {
    __asm
        push    ix
        ld      b, #0x30        ; DSS_WAITKEY
        ld      c, #0x35        ; DSS_K_CLEAR: flush buffered keys, then wait
        rst     #0x10
        pop     ix
        ret
    __endasm;
}

void main(void) {
    u8 mode;
    u8 page;
    u8 rc;

    dss_clrscr();
    dss_getvmod(&mode, &page);

    printf("=== CONSOLE / VIDEO demo ===\n\n");
    printf("Current DSS mode : %u\n", (u16)mode);
    printf("Current video page: %u\n", (u16)page);

    rc = dss_setvmod(mode, page);
    printf("dss_setvmod(current,current) -> %u\n", (u16)rc);

    print_status(6, "Building a text window...");

    /* DSS CLEAR expects B=attr and A=fill char. */
    dss_clear(4, 8, 34, 6, 0x1F, ' ');
    print_line(5, 9,  "Line 1: top");
    print_line(5, 10, "Line 2: scroll me");
    print_line(5, 11, "Line 3: scroll me");
    print_line(5, 12, "Line 4: scroll me");
    print_line(5, 13, "Line 5: bottom");

    print_status(16, "Press any key to scroll the window...");
    wait_fresh_key();

    dss_scroll(4, 8, 34, 6, SCROLL_UP, 1);
    print_line(5, 13, "New bottom line");

    print_status(16, "                                        ");
    print_status(17, "Press any key...");
    wait_fresh_key();
}
