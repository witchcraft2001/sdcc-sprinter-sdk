/* stdarg.h - ANSI variable argument support for SDCC 2.9.0 / Z80 */
#ifndef _STDARG_H
#define _STDARG_H

typedef unsigned char *va_list;

#define va_start(marker, last) \
    { (marker) = (va_list)&(last) + sizeof(last); }

#define va_arg(marker, type) \
    *((type *)(((marker) += sizeof(type)) - sizeof(type)))

#define va_end(marker) \
    { (marker) = (va_list)0; }

#endif /* _STDARG_H */
