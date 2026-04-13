#include <stdio.h>
#include <sprinter/dss.h>

void main(void) {
    dss_find_t entry;
    printf("Directory listing: *.*\n\n");
    if (dss_ffirst("*.*", &entry, 0x20) == 0) {
        do {
            printf("%s\n", entry.ff_name);
        } while (dss_fnext(&entry) == 0);
    }
    printf("\nDone. Press any key.\n");
    getchar();
}
