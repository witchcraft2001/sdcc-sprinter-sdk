/* feof_ferror.c — file status functions */
#include <stdio.h>

int feof(FILE *fp) {
    return (fp->flags & _F_EOF) ? 1 : 0;
}

int ferror(FILE *fp) {
    return (fp->flags & _F_ERR) ? 1 : 0;
}

void rewind(FILE *fp) {
    fseek(fp, 0L, SEEK_SET);
    fp->flags &= ~(_F_EOF | _F_ERR);
}
