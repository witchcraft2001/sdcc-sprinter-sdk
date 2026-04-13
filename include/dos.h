/**
 * dos.h — DOS/system interface (Turbo C / SOLID C compatible)
 *
 * Zero-overhead macros where possible; thin wrappers only when
 * type conversion is needed.
 */

#ifndef _DOS_H
#define _DOS_H

#include <sprinter/dss.h>

/* Turbo C compatible date/time structures (binary layout matches dss_*_t) */
struct date {
    int  da_year;
    char da_day;
    char da_mon;
};

struct time {
    unsigned char ti_min;
    unsigned char ti_hour;
    unsigned char ti_hund;
    unsigned char ti_sec;
};

/* Interrupt control — direct macro aliases (zero overhead) */
#define enable()    dss_ei()
#define disable()   dss_di()

/* Disk — direct macro aliases */
#define getdisk()       dss_getdisk()
#define setdisk(d)      dss_setdisk(d)

/* Date/time — zero overhead (struct layout matches dss_date_t/dss_time_t) */
#define getdate(d)      dss_getdate((dss_date_t*)(d))
#define gettime(t)      dss_gettime((dss_time_t*)(t))

#endif /* _DOS_H */
