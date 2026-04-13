#include <conio.h>
#include <sprinter/dss.h>

void gotoxy(int x, int y) {
    dss_gotoxy((u8)x, (u8)y);
}
