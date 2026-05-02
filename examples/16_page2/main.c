/**
 * 16_page2 -- CRT0_PAGE2 runtime layout smoke test
 */

#include <stdio.h>
#include <string.h>
#include <sprinter.h>

static const char init_msg[] = "INITOK";
static unsigned char bss_buf[64];

static int check_bss(void) {
    unsigned char i;

    for (i = 0; i < sizeof(bss_buf); i++) {
        if (bss_buf[i] != 0)
            return 0;
    }
    return 1;
}

static unsigned int stack_probe(void) {
    unsigned char local[96];
    unsigned char i;
    unsigned int sum = 0;

    for (i = 0; i < sizeof(local); i++) {
        local[i] = i;
        sum += local[i];
    }
    return sum;
}

void main(void) {
    u16 total = 0;
    u16 free_pages = 0;
    int ok = 1;

    dss_clrscr();
    printf("=== CRT0_PAGE2 test ===\n\n");

    printf("init data : %s\n", init_msg);
    ok = ok && (strcmp(init_msg, "INITOK") == 0);

    printf("bss zero  : %s\n", check_bss() ? "OK" : "FAIL");
    ok = ok && check_bss();

    printf("stack sum : %u\n", stack_probe());
    ok = ok && (stack_probe() == 4560);

    dss_meminfo(&total, &free_pages);
    printf("mem pages : total=%u free=%u\n", total, free_pages);

    printf("\nRESULT: %s\n", ok ? "OK" : "FAIL");
    printf("Press any key...\n");
    getchar();
}
