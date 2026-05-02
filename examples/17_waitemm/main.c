/**
 * 17_waitemm -- dss_wait() and bios_emm_list() smoke test
 */

#include <stdio.h>
#include <sprinter.h>
#include <sprinter/bios.h>

static void print_pages(const unsigned char *pages, unsigned char count) {
    unsigned char i;

    for (i = 0; i < count; i++) {
        printf(" %u", (unsigned int)pages[i]);
    }
    printf("\n");
}

void main(void) {
    unsigned char block;
    unsigned char pages[8];
    unsigned char count;
    unsigned char wait_rc;
    signed int exec_rc;
    unsigned char err = 0;

    dss_clrscr();
    printf("=== WAIT / EMM test ===\n\n");

    block = dss_getmem_pages(2);
    if (block == 0xFF) {
        printf("dss_getmem_pages(2): FAIL\n");
    } else {
        count = bios_emm_list(block, pages);
        printf("block id : %u\n", (unsigned int)block);
        printf("emm count: %u\n", (unsigned int)count);
        printf("emm pages:");
        print_pages(pages, count < sizeof(pages) ? count : sizeof(pages));
        dss_freemem(block);
    }

    printf("\nRunning WCHILD.EXE...\n");
    exec_rc = dss_exec_ex("WCHILD.EXE", &err);
    wait_rc = dss_wait();

    if (exec_rc < 0) {
        printf("exec failed: err=%u\n", (unsigned int)err);
    } else {
        printf("exec rc : %d\n", exec_rc);
        printf("wait rc : %u\n", (unsigned int)wait_rc);
        printf("RESULT  : %s\n", wait_rc == 7 ? "OK" : "FAIL");
    }

    printf("\nPress any key...\n");
    getchar();
}
