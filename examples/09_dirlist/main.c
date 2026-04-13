#include <stdio.h>
#include <dir.h>

void main(void) {
    ffblk entry;
    printf("Directory listing: *.*\n\n");
    if (findfirst("*.*", &entry, 0x20) == 0) {
        do {
            printf("%s\n", entry.ff_name);
        } while (findnext(&entry) == 0);
    }
    printf("\nDone. Press any key.\n");
    getchar();
}
