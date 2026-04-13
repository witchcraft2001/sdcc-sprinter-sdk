/* ungetc.c */
#include <stdio.h>

int ungetc(int c, FILE *fp) {
    if (c == EOF || fp->ungetc_buf != 0xFF)
        return EOF;
    fp->ungetc_buf = (unsigned char)c;
    fp->flags &= ~_F_EOF;
    return c;
}
