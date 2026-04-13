#include <stdio.h>

void main(void) {
    FILE *fp;
    char buf[80];
    int i;

    printf("=== fprintf test ===\n\n");

    fp = fopen("DATA.TXT", "w");
    if (!fp) { printf("Error creating file\n"); return; }

    for (i = 1; i <= 5; i++) {
        fprintf(fp, "Line %d: value=%d hex=0x%X\n", i, i * 100, i * 100);
    }
    fclose(fp);
    printf("Written 5 lines to DATA.TXT\n\n");

    fp = fopen("DATA.TXT", "r");
    if (!fp) { printf("Error reading file\n"); return; }

    printf("File contents:\n");
    while (fgets(buf, 79, fp)) {
        printf("%s", buf);
    }
    fclose(fp);

    remove("DATA.TXT");
    printf("\nFile deleted. Press any key.\n");
    getchar();
}
