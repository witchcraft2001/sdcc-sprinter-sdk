/**
 * 26_iotest -- Regression checks for stdio file modes and character I/O
 */

#include <stdio.h>
#include <string.h>

static int fails;

static void check(int ok, const char *name) {
    printf("%s: %s\n", name, ok ? "OK" : "FAIL");
    if (!ok) fails++;
}

static int read_file(char *buf, int size) {
    FILE *fp;
    int n;

    memset(buf, 0, size);
    fp = fopen("IOTEST.TXT", "rb");
    if (!fp) return -1;
    n = (int)fread(buf, 1, size - 1, fp);
    fclose(fp);
    return n;
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

    fp = fopen("IOTEST.TXT", "wt");
    check(fp != NULL, "fopen wt");
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

    fp = fopen("IOTEST.TXT", "rb");
    check(fp != NULL, "fopen rb");
    if (fp) {
        c = fgetc(fp);
        check(c == 'A', "fgetc");
        rc = ungetc(c, fp);
        check(rc == c, "ungetc");
        memset(buf, 0, sizeof(buf));
        n = (int)fread(buf, 1, 6, fp);
        check(n == 6 && strcmp(buf, "ABCDEF") == 0, "fread rb");
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
        memset(buf, 0, sizeof(buf));
        n = (int)fread(buf, 1, 2, fp);
        check(n == 2 && strcmp(buf, "BC") == 0, "seekbuf fread");
        fclose(fp);
    }

    fp = fopen("IOTEST.TXT", "r+b");
    check(fp != NULL, "fopen r+b");
    if (fp) {
        rc = fseek(fp, 2L, SEEK_SET);
        check(rc == 0, "fseek set");
        n = (int)fwrite("xy", 1, 2, fp);
        check(n == 2, "r+b write");
        fclose(fp);
    }

    fp = fopen("IOTEST.TXT", "rb+");
    check(fp != NULL, "fopen rb+");
    if (fp) {
        rc = fseek(fp, 5L, SEEK_SET);
        check(rc == 0, "fseek rb+");
        rc = fputc('Z', fp);
        check(rc == 'Z', "rb+ fputc");
        fclose(fp);
    }

    fp = fopen("IOTEST.TXT", "ab+");
    check(fp != NULL, "fopen ab+");
    if (fp) {
        rc = fseek(fp, 0L, SEEK_SET);
        check(rc == 0, "append seek");
        rc = fputc('Q', fp);
        check(rc == 'Q', "append forced eof");
        fclose(fp);
    }

    n = read_file(buf, sizeof(buf));
    check(n == 7 && strcmp(buf, "ABxyEZQ") == 0, "final content");

    fp = fopen("iotest.txt", "rb");
    check(fp != NULL, "lowercase open");
    if (fp) {
        memset(buf, 0, sizeof(buf));
        n = (int)fread(buf, 1, 7, fp);
        check(n == 7 && strcmp(buf, "ABxyEZQ") == 0, "lowercase read");
        fclose(fp);
    }

    fp = fopen("IOTEST.TXT", "rb");
    check(fp != NULL, "readonly open");
    if (fp) {
        n = (int)fwrite("!", 1, 1, fp);
        check(n == 0, "readonly fwrite");
        fclose(fp);
    }

    fp = fopen("IOTEST.TXT", "wb");
    check(fp != NULL, "writeonly open");
    if (fp) {
        n = (int)fread(buf, 1, 1, fp);
        check(n == 0, "writeonly fread");
        fclose(fp);
    }

    remove("iotest.txt");

    if (fails) {
        printf("\nResult: FAIL (%d failures)\n", fails);
    } else {
        printf("\nResult: OK\n");
    }
    printf("Press any key...\n");
    getchar();
}
