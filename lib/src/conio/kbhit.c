#include <conio.h>
#include <sprinter/dss.h>

int kbhit(void) {
    return dss_kbhit() ? 1 : 0;
}
