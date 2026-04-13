#include <stdio.h>
#include <string.h>

void main(void) {
    FILE *fin;
    char fname[32];
    int c, count = 0;

    printf("BIN2C - Binary to C array\n");
    printf("Input file: ");
    /* Read filename from keyboard */
    {
        int i = 0;
        while (i < 30) {
            c = getchar();
            if (c == '\n' || c == '\r') break;
            putchar(c);
            fname[i++] = c;
        }
        fname[i] = 0;
        putchar('\n');
    }

    fin = fopen(fname, "r");
    if (!fin) {
        printf("Can't open: %s\n", fname);
        return;
    }

    printf("\nunsigned char data[] = {\n  ");
    while ((c = fgetc(fin)) != EOF) {
        if (count > 0) printf(",");
        if (count > 0 && (count % 12) == 0) printf("\n  ");
        printf("0x%02X", c);
        count++;
    }
    printf("\n};\n// %d bytes\n", count);
    fclose(fin);

    printf("\nDone. Press any key.\n");
    getchar();
}
