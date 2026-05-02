/**
 * 18_iotest -- stdio regression checks
 */

#include <stdio.h>
#include <string.h>

static int fails;

static void check(int ok, const char *name) {
    printf("%s: %s\n", name, ok ? "OK" : "FAIL");
    if (!ok)
        fails++;
}

void main(void) {
    FILE *fp;
    char buf[32];
    int n;
    int c;
    int rc;

    fails = 0;
    printf("=== stdio I/O regression ===\n\n");

    remove("IOTEST.TXT");

    fp = fopen("iotest.txt", "wt");
    check(fp != NULL, "fopen lower wt");
    if (fp) {
        rc = fputc('A', fp);
        check(rc == 'A', "fputc");
        rc = fputs("BC", fp);
        check(rc >= 0, "fputs");
        n = (int)fwrite("DE", 1, 2, fp);
        check(n == 2, "fwrite");
        rc = fclose(fp);
        check(rc == 0, "fclose wt");
    }

    fp = fopen("IOTEST.TXT", "ab");
    check(fp != NULL, "fopen ab");
    if (fp) {
        rc = fputc('F', fp);
        check(rc == 'F', "append fputc");
        fclose(fp);
    }

    fp = fopen("iotest.txt", "rb");
    check(fp != NULL, "fopen lower rb");
    if (fp) {
        c = fgetc(fp);
        check(c == 'A', "fgetc");
        rc = ungetc(c, fp);
        check(rc == c, "ungetc");
        memset(buf, 0, sizeof(buf));
        n = (int)fread(buf, 1, 6, fp);
        check(n == 6 && strcmp(buf, "ABCDEF") == 0, "fread ungetc");
        fclose(fp);
    }

    fp = fopen("IOTEST.TXT", "rb");
    check(fp != NULL, "fopen seekbuf");
    if (fp) {
        c = fgetc(fp);
        rc = ungetc(c, fp);
        check(rc == c, "seekbuf ungetc");
        rc = fseek(fp, 1L, SEEK_SET);
        check(rc == 0, "seek clears ungetc");
        c = fgetc(fp);
        check(c == 'B', "fgetc after seek");
        fclose(fp);
    }

    printf("\nFailures: %d\n", fails);
    printf("Press any key...\n");
    getchar();
}
