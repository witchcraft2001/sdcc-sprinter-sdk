/* puts.c — print string + newline to stdout
 * Goes through fputc for \n -> \r\n conversion.
 * SDCC may optimize printf("str\n") -> puts("str"),
 * so puts must handle newlines correctly. */
#include <stdio.h>

int puts(const char *s) {
    while (*s)
        fputc(*s++, stdout);
    fputc('\n', stdout);
    return 0;
}
