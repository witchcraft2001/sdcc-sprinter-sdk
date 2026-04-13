#include <conio.h>
#include <sprinter/dss.h>

int getche(void) {
    return (int)(dss_getche() & 0xFF);
}
