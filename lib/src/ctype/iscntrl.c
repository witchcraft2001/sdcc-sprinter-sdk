#include <ctype.h>

int iscntrl(int c) {
    return c < 32 || c == 127;
}
