#include <stdlib.h>
#include <sprinter/dss.h>

void exit(int status) {
    dss_exit((u8)status);
}
