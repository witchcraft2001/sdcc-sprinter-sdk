/*
 * 37_win0big - multi-page: code/rodata spanning WIN0 + WIN1.
 *
 * bigtab is ~16 KB of 0xA5 in rodata; it runs past 0x4000 into WIN1. The
 * checksum over the whole array only matches if the WIN1 blob was streamed in
 * by the multi-page loader. Ordinary printf code, one-line Makefile.
 */

#include <stdio.h>
#include <sprinter.h>

#define R16(x)  x,x,x,x,x,x,x,x,x,x,x,x,x,x,x,x
#define R256(x) R16(R16(x))
#define R4K(x)  R16(R256(x))

static const u8 bigtab[] = { R4K(0xA5), R4K(0xA5), R4K(0xA5), R4K(0xA5) };

void main(void) {
    u16 i, sum = 0, expect;

    printf("== multi-page code/rodata (WIN0+WIN1) ==\r\n");
    printf("bigtab 0x%04X..0x%04X (%u bytes)\r\n",
           (u16)bigtab, (u16)(bigtab + sizeof(bigtab) - 1), (u16)sizeof(bigtab));

    for (i = 0; i < sizeof(bigtab); i++) sum = (u16)(sum + bigtab[i]);
    expect = (u16)(sizeof(bigtab) * 0xA5);

    printf("sum=0x%04X expect=0x%04X -> %s\r\n",
           sum, expect, (sum == expect) ? "WIN1 verified, multi-page OK" : "MISMATCH");
    printf("Press a key.\r\n");

    dss_waitkey();
}
