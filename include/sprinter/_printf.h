/* _printf.h — internal: shared printf engine */
#ifndef _SPRINTER_PRINTF_H
#define _SPRINTER_PRINTF_H
#include <stdio.h>
#include <stdarg.h>
int _vprintfmt(FILE *fp, const char *fmt, va_list ap);
#endif
