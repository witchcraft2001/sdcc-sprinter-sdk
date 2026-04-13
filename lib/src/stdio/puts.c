/* puts.c — print string + newline to stdout */
#include <stdio.h>
#include <sprinter/dss.h>

int puts(const char *s) {
    dss_puts(s);
    dss_putchar('\n');
    return 0;
}
