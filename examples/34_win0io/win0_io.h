/*
 * win0_io.h - pointer-marshalling ("bounce") wrappers for the extended
 * WIN0..WIN2 layout (Phase 3, docs/ru/11_extended_layout.md 11.5).
 *
 * During a DSS/BIOS call the trampoline puts the DSS core page into WIN0, so
 * any pointer passed to firmware must NOT point into WIN0 (0x0000-0x3FFF).
 * These wrappers gate on `ptr < 0x4000`: pointers in WIN1/WIN2/WIN3 are
 * passed straight through; WIN0 pointers are copied via a WIN2 staging buffer
 * (input copied before the call, output copied after, streams chunked).
 */
#ifndef WIN0_IO_H
#define WIN0_IO_H

#include <sprinter/types.h>

void w_puts(const char *s);              /* input string (chunked)       */
void w_puthex(u16 v);                    /* 4-hex-digit helper           */
u8   w_curdir(char *buf);                /* output buffer                */
i16  w_creat(const char *path);          /* input filename               */
i16  w_open(const char *path, u8 mode);  /* input filename               */
i16  w_read(u8 fd, void *buf, u16 count);   /* output buffer (chunked)   */
i16  w_write(u8 fd, const void *buf, u16 count); /* input data (chunked) */

#endif /* WIN0_IO_H */
