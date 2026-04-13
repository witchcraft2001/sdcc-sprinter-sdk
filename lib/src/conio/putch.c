#include <conio.h>
#include <sprinter/dss.h>

int putch(int c) {
    dss_putchar((u8)c);
    return c;
}
