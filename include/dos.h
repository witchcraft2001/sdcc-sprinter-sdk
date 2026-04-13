/**
 * dos.h — DOS/system interface (Turbo C / SOLID C compatible)
 *
 * Zero-overhead macros where possible; thin wrappers only when
 * type conversion is needed.
 */

#ifndef _DOS_H
#define _DOS_H

#include <sprinter/dss.h>

/* Interrupt control — direct macro aliases (zero overhead) */
#define enable()    dss_ei()
#define disable()   dss_di()

/* Disk — direct macro aliases */
#define getdisk()       dss_getdisk()
#define setdisk(d)      dss_setdisk(d)

#endif /* _DOS_H */
