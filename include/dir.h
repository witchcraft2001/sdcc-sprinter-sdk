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

/* Directory operations — zero-overhead macros */
#define chdir(path)                 dss_chdir(path)
#define findfirst(pat, res, attr)   dss_ffirst((pat), (dss_find_t*)(res), (u8)(attr))
#define findnext(res)               dss_fnext((dss_find_t*)(res))

/* SOLID C aliases */
#define ffirst  findfirst
#define fnext   findnext

#endif /* _DIR_H */
