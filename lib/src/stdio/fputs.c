/* fputs.c — write string to stream */
#include <stdio.h>

int fputs(const char *s, FILE *fp) {
    while (*s) {
        if (fputc(*s++, fp) == EOF)
            return EOF;
    }
    return 0;
}
