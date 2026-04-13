/* ftell.c */
#include <stdio.h>

long ftell(FILE *fp) {
    /* DSS doesn't have a direct "tell" function.
       Would need seek(0, SEEK_CUR) to get position.
       For now, return -1 (not supported). */
    (void)fp;
    return -1L;
}
