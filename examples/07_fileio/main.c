#include <stdio.h>
#include <string.h>

void main(void) {
    FILE *fp;
    char buf[64];
    const char *msg = "Hello from stdio!\n";

    printf("=== stdio File I/O ===\n\n");

    fp = fopen("TEST.TXT", "w");
    if (!fp) { printf("Error: can't create\n"); getchar(); return; }
    fwrite(msg, 1, strlen(msg), fp);
    fclose(fp);
    printf("Written %d bytes\n", strlen(msg));

    fp = fopen("TEST.TXT", "r");
    if (!fp) { printf("Error: can't open\n"); getchar(); return; }
    memset(buf, 0, sizeof(buf));
    fread(buf, 1, 63, fp);
    fclose(fp);
    printf("Read back: %s\n", buf);

    remove("TEST.TXT");
    printf("File deleted.\n\nDone! Press any key.\n");
    getchar();
}
