#include <conio.h>
#include <sprinter/dss.h>

int getch(void) {
    return (int)dss_waitkey();
}
