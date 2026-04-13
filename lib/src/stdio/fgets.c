/* fgets.c — read line from stream */
#include <stdio.h>

char *fgets(char *buf, int size, FILE *fp) {
    int i = 0, c;
    if (size <= 0) return NULL;
    while (i < size - 1) {
        c = fgetc(fp);
        if (c == EOF) {
            if (i == 0) return NULL;
            break;
        }
        buf[i++] = (char)c;
        if (c == '\n') break;
    }
    buf[i] = '\0';
    return buf;
}
