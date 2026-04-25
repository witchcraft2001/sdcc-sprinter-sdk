/**
 * 18_call -- dss_call() / dss_callp() demo
 */

#include <stdio.h>
#include <sprinter.h>

static u16 return_magic(void) {
    return 0x1234;
}

static u16 add_0x1111(u16 value) {
    return value + 0x1111;
}

static u16 xor_mask(u16 value) {
    return value ^ 0x00FF;
}

void main(void) {
    u16 rc1;
    u16 rc2;
    u16 rc3;

    dss_clrscr();
    printf("=== CALL / CALLP demo ===\r\n\r\n");

    rc1 = dss_call((u16)return_magic);
    rc2 = dss_callp((u16)add_0x1111, 0x2222);
    rc3 = dss_callp((u16)xor_mask, 0x55AA);

    printf("dss_call(return_magic)        = 0x%X\r\n", rc1);
    printf("dss_callp(add_0x1111, 0x2222) = 0x%X\r\n", rc2);
    printf("dss_callp(xor_mask,   0x55AA) = 0x%X\r\n", rc3);

    printf("\r\nExpected values:\r\n");
    printf("0x1234, 0x3333, 0x5555\r\n");

    printf("\r\nPress any key...\r\n");
    getchar();
}
