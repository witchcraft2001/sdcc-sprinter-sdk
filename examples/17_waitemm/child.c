/**
 * WCHILD.EXE -- child process for dss_wait() test
 */

#include <sprinter.h>
#include <sprinter/dss.h>

int main(void) {
    dss_puts("WCHILD: returning 7\r\n");
    dss_exit(7);
    return 7;
}
