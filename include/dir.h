/**
 * dir.h — Directory operations (Turbo C / SOLID C compatible)
 *
 * Macros for zero-overhead aliases where signatures match.
 */

#ifndef _DIR_H
#define _DIR_H

#include <sprinter/dss.h>

/* File find structure aliases */
typedef dss_find_t FIND;    /* SOLID C compat */
typedef dss_find_t ffblk;   /* Turbo C compat */

/* File attribute constants */
#ifndef FA_NORMAL
#define FA_NORMAL   0x00
#define FA_RDONLY    0x01
#define FA_HIDDEN   0x02
#define FA_SYSTEM   0x04
#define FA_LABEL    0x08
#define FA_DIREC    0x10
#define FA_ARCH     0x20
#endif

/* fnsplit() return flags */
#define WILDCARDS   0x01
#define EXTENSION   0x02
#define FILENAME    0x04
#define DIRECTORY   0x08
#define DRIVE       0x10

/* Directory operations — zero-overhead macros */
#define chdir(path)                 dss_chdir(path)
#define mkdir(path)                 dss_mkdir(path)
#define rmdir(path)                 dss_rmdir(path)
#define getcurdir(drv, buf)         ((void)(drv), (int)dss_curdir(buf))
#define findfirst(pat, res, attr)   dss_ffirst((pat), (dss_find_t*)(res), (u8)(attr))
#define findnext(res)               dss_fnext((dss_find_t*)(res))

/* POSIX compatible — thin wrappers (different return type) */
char *getcwd(char *buf, int size);

/* Path splitting (Turbo C compatible) */
int fnsplit(const char *path, char *drive, char *dir,
            char *fname, char *ext);

/* SOLID C aliases */
#define ffirst  findfirst
#define fnext   findnext

#endif /* _DIR_H */
