/**
 * dss.h — Estex DSS (Disk SubSystem) OS API for ZX Sprinter
 *
 * System calls via RST #10 vector.
 * Call convention: function number in C register, params in A/B/D/E/H/L/IX/IY.
 * Returns: CF=0 success, CF=1 error (code in A).
 */

#ifndef _SPRINTER_DSS_H
#define _SPRINTER_DSS_H

#include <sprinter/types.h>

/* ===== DSS Function Numbers ===== */

/* System */
#define DSS_VERSION     0x00
#define DSS_ENABLE      0x01    /* Enable interrupts */
#define DSS_GETDISK     0x02    /* Get current disk */
#define DSS_SETDISK     0x03    /* Set current disk (in custom impl) */
#define DSS_GETENV      0x10    /* Get environment */

/* Files */
#define DSS_CREAT       0x0A    /* Create file */
#define DSS_OPEN        0x11    /* Open file */
#define DSS_CLOSE       0x12    /* Close file */
#define DSS_READ        0x13    /* Read from file */
#define DSS_WRITE       0x14    /* Write to file */
#define DSS_SEEK        0x15    /* Seek in file */
#define DSS_DELETE      0x0E    /* Delete file */
#define DSS_RENAME      0x10    /* Rename file */

/* Directories */
#define DSS_FFIRST      0x19    /* Find first file */
#define DSS_FNEXT       0x1A    /* Find next file */
#define DSS_MKDIR       0x1B    /* Create directory */
#define DSS_CHDIR       0x1D    /* Change directory */
#define DSS_CURDIR      0x1E    /* Get current directory */

/* Time */
#define DSS_SYSTIME     0x21    /* Get system time/date */
#define DSS_SETTIME     0x22    /* Set system time/date */

/* Keyboard */
#define DSS_WAITKEY     0x30    /* Wait for key press */
#define DSS_SCANKEY     0x31    /* Scan keyboard (non-blocking) */
#define DSS_GETCHE      0x32    /* Get key with echo */
#define DSS_KBHIT       0x33    /* Check if key pressed */

/* Memory */
#define DSS_SETWIN      0x38    /* Set memory window page */
#define DSS_GETMEM      0x3D    /* Allocate memory page */
#define DSS_FREEMEM     0x3E    /* Free memory page */

/* Process */
#define DSS_EXEC        0x40    /* Execute program */
#define DSS_EXIT        0x41    /* Exit program */

/* Video */
#define DSS_SETVMOD     0x50    /* Set video mode */
#define DSS_GETVMOD     0x51    /* Get video mode */
#define DSS_LOCATE      0x52    /* Set cursor position */
#define DSS_PUTCHAR     0x5B    /* Output character */
#define DSS_PUTSTR      0x5C    /* Output string */
#define DSS_SCROLL      0x55    /* Scroll screen */
#define DSS_CLEAR       0x56    /* Clear screen area */

/* ===== File Open Modes ===== */
#define O_RDONLY        0x00
#define O_WRONLY        0x01
#define O_RDWR          0x02
#define O_CREAT         0x04
#define O_TRUNC         0x08
#define O_APPEND        0x10

/* ===== File Attributes ===== */
#define FA_NORMAL       0x00
#define FA_RDONLY        0x01
#define FA_HIDDEN       0x02
#define FA_SYSTEM       0x04
#define FA_LABEL        0x08
#define FA_DIREC        0x10
#define FA_ARCH         0x20

/* ===== Seek Origins ===== */
#define SEEK_SET        0
#define SEEK_CUR        1
#define SEEK_END        2

/* ===== Structures ===== */

/** Directory entry from ffirst/fnext */
typedef struct {
    char     name[8];       /* +0  filename pattern */
    char     ext[3];        /* +8  extension pattern */
    u8       attrib;        /* +11 search attribute */
    u8       reserved[10];  /* +12 reserved */
    u16      time;          /* +22 file time */
    u16      date;          /* +24 file date */
    u16      cluster;       /* +26 first cluster */
    u16      size_lo;       /* +28 file size low word */
    u16      size_hi;       /* +30 file size high word */
    u8       attr;          /* +32 matched file attribute */
    char     ff_name[223];  /* +33 matched filename (null-terminated) */
} dss_find_t;

/** System date */
typedef struct {
    u16      year;
    u8       day;
    u8       month;
} dss_date_t;

/** System time */
typedef struct {
    u8       minute;
    u8       hour;
    u8       hundredths;
    u8       second;
} dss_time_t;


/* ===== Functions ===== */

/** Exit program with error code */
void dss_exit(u8 code);

/** Print a single character to console */
void dss_putchar(u8 ch);

/** Print a null-terminated string to console */
void dss_puts(const char *str);

/** Wait for a key press, return character code */
u8 dss_waitkey(void);

/** Check if a key is available in keyboard buffer */
bool dss_kbhit(void);

/** Get key with echo */
u16 dss_getche(void);

/** Set cursor position (1-based) */
void dss_gotoxy(u8 x, u8 y);

/** Clear screen */
void dss_clrscr(void);

/** Get current disk number */
u8 dss_getdisk(void);

/** Set current disk */
void dss_setdisk(u8 disk);

/** Open file, returns file descriptor or -1 on error */
i16 dss_open(const char *path, u8 mode);

/** Create new file, returns file descriptor or -1 on error */
i16 dss_creat(const char *path);

/** Close file descriptor */
u8 dss_close(u8 fd);

/** Read from file. Returns bytes actually read, or -1 on error */
i16 dss_read(u8 fd, void *buf, u16 count);

/** Write to file. Returns bytes actually written, or -1 on error */
i16 dss_write(u8 fd, const void *buf, u16 count);

/** Seek in file. Returns 0 on success, -1 on error */
i16 dss_seek(u8 fd, u32 offset, u8 origin);

/** Delete file. Returns 0 on success */
u8 dss_delete(const char *path);

/** Rename file */
u8 dss_rename(const char *oldpath, const char *newpath);

/** Change directory */
u8 dss_chdir(const char *path);

/** Find first matching file */
i8 dss_ffirst(const char *pattern, dss_find_t *result, u8 attr);

/** Find next matching file */
i8 dss_fnext(dss_find_t *result);

/** Get system date */
void dss_getdate(dss_date_t *d);

/** Get system time */
void dss_gettime(dss_time_t *t);

/** Enable interrupts */
void dss_ei(void);

/** Disable interrupts */
void dss_di(void);

/** Set memory window page.
 *  win: 0-3 (memory window number)
 *  page: RAM page number
 */
void dss_setwin(u8 win, u8 page);

/** Allocate a memory page. Returns page number or 0xFF on error */
u8 dss_getmem(void);

/** Free a memory page */
void dss_freemem(u8 page);

/** Execute a program. Returns exit code */
i16 dss_exec(const char *path);

/** Get pointer to saved command line */
char *dss_cmdline(void);

#endif /* _SPRINTER_DSS_H */
