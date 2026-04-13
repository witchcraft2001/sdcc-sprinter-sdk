#include <conio.h>
#include <sprinter/dss.h>
#include <sprinter/_printf.h>
#include <stdarg.h>

int cprintf(const char *fmt, ...) {
    va_list ap;
    int r;
    va_start(ap, fmt);
    r = _vprintfmt(stdout, fmt, ap);
    va_end(ap);
    return r;
}
