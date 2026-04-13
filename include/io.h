/**
 * io.h — Low-level file I/O (POSIX/Turbo C compatible)
 *
 * Zero-overhead macros mapping to dss_* functions.
 * Type casts handle int→u8 fd conversion silently.
 */

#ifndef _IO_H
#define _IO_H

#include <sprinter/dss.h>

/* File open flags (re-export from dss.h) */
#ifndef O_RDONLY
#define O_RDONLY    0x00
#define O_WRONLY    0x01
#define O_RDWR      0x02
#define O_CREAT     0x04
#define O_TRUNC     0x08
#define O_APPEND    0x10
#endif

/* File descriptor type (SOLID C compat) */
typedef int FD;

/* Low-level file I/O — zero-overhead macros */
#define open(path, mode)        ((int)dss_open((path), (u8)(mode)))
#define close(fd)               ((int)dss_close((u8)(fd)))
#define read(fd, buf, count)    ((int)dss_read((u8)(fd), (buf), (u16)(count)))
#define write(fd, buf, count)   ((int)dss_write((u8)(fd), (buf), (u16)(count)))
#define lseek(fd, off, whence)  ((long)dss_seek((u8)(fd), (u32)(off), (u8)(whence)))

#endif /* _IO_H */
