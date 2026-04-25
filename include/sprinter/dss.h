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
#define DSS_VERSION     0x00    /* Get DSS version */
#define DSS_SETDISK     0x01    /* Set current disk */
#define DSS_GETDISK     0x02    /* Get current disk */
#define DSS_DISKINF     0x03    /* Get disk info */

/* Files */
#define DSS_CREAT       0x0A    /* Create file */
#define DSS_CREAT_N     0x0B    /* Create new (fail if exists) */
#define DSS_DELETE      0x0E    /* Delete file */
#define DSS_RENAME      0x10    /* Rename file */
#define DSS_OPEN        0x11    /* Open file */
#define DSS_CLOSE       0x12    /* Close file */
#define DSS_READ        0x13    /* Read from file */
#define DSS_WRITE       0x14    /* Write to file */
#define DSS_SEEK        0x15    /* Seek in file */
#define DSS_ATTRIB      0x16    /* Get/set file attributes */
#define DSS_GET_D_T     0x17    /* Get file date/time */
#define DSS_PUT_D_T     0x18    /* Set file date/time */

/* Directories */
#define DSS_FFIRST      0x19    /* Find first file */
#define DSS_FNEXT       0x1A    /* Find next file */
#define DSS_MKDIR       0x1B    /* Create directory */
#define DSS_RMDIR       0x1C    /* Remove directory */
#define DSS_CHDIR       0x1D    /* Change directory */
#define DSS_CURDIR      0x1E    /* Get current directory */

/* Time */
#define DSS_SYSTIME     0x21    /* Get system date/time */
#define DSS_SETTIME     0x22    /* Set system date/time */

/* Keyboard */
#define DSS_WAITKEY     0x30    /* Wait for key press */
#define DSS_SCANKEY     0x31    /* Scan keyboard (non-blocking) */
#define DSS_GETCHE      0x32    /* Get key with echo */
#define DSS_KBHIT       0x33    /* Check if key pressed */
#define DSS_K_CLEAR     0x35    /* Clear keyboard buffer */
#define DSS_TESTKEY     0x37    /* Test keyboard buffer (non-destructive) */

/* Memory */
#define DSS_SETWIN      0x38    /* Set memory window page */
#define DSS_MEMINFO     0x3C    /* Get memory info (total/free) */
#define DSS_GETMEM      0x3D    /* Allocate memory page */
#define DSS_FREEMEM     0x3E    /* Free memory page */
#define DSS_SETMEM      0x3F    /* Resize memory block */

/* Process */
#define DSS_EXEC        0x40    /* Execute program */
#define DSS_EXIT        0x41    /* Exit program */
#define DSS_WAIT        0x42    /* Wait for child process */
#define DSS_DOSNAME     0x44    /* Get DSS file name */
#define DSS_EX_PATH     0x45    /* Parse path components */
#define DSS_ENVIRON     0x46    /* Environment variables */
#define DSS_APPINFO     0x47    /* Application information */

/* Video */
#define DSS_SETVMOD     0x50    /* Set video mode */
#define DSS_GETVMOD     0x51    /* Get video mode */
#define DSS_LOCATE      0x52    /* Set cursor position */
#define DSS_CURSOR      0x53    /* Cursor control */
#define DSS_SELPAGE     0x54    /* Select video page */
#define DSS_SCROLL      0x55    /* Scroll screen area */
#define DSS_CLEAR       0x56    /* Clear screen area */
#define DSS_PUTCHAR     0x5B    /* Output character */
#define DSS_PUTSTR      0x5C    /* Output string */

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

/** Keyboard event returned by WAITKEY/SCANKEY/TESTKEY */
typedef struct {
    u8       ascii;       /* ASCII code */
    u8       scan;        /* Position/scan code */
    u8       modifiers;   /* Shift/Ctrl/Alt state */
    u8       locks;       /* Caps/Num/Scroll/etc. state */
} dss_key_t;


/* ===== EX_PATH subfunctions ===== */
#define EXPATH_ALL      0       /* Extract all components */
#define EXPATH_DRIVE    1       /* Extract drive */
#define EXPATH_PATH     2       /* Extract path */
#define EXPATH_NAME     3       /* Extract filename */
#define EXPATH_EXT      4       /* Extract extension */

/* ===== APPINFO subfunctions ===== */
#define APPINFO_PARAMS  0       /* Get command line parameters */
#define APPINFO_DIR     1       /* Get application directory */
#define APPINFO_FULL    2       /* Get full application path */

/* ===== Scroll directions ===== */
#define SCROLL_UP       1
#define SCROLL_DOWN     2

/* ===== Keyboard modifier bits (dss_key_t.modifiers) ===== */
#define DSS_KEYMOD_RALT     0x01
#define DSS_KEYMOD_RCTRL    0x02
#define DSS_KEYMOD_LALT     0x04
#define DSS_KEYMOD_LCTRL    0x08
#define DSS_KEYMOD_ALT      0x10
#define DSS_KEYMOD_CTRL     0x20
#define DSS_KEYMOD_RSHIFT   0x40
#define DSS_KEYMOD_LSHIFT   0x80

/* ===== Keyboard lock bits (dss_key_t.locks) ===== */
#define DSS_KEYLOCK_CAPS    0x01
#define DSS_KEYLOCK_INSERT  0x02
#define DSS_KEYLOCK_SCROLL  0x04
#define DSS_KEYLOCK_NUM     0x08
#define DSS_KEYLOCK_RUSLAT  0x80

/* ===== Functions ===== */

/* --- System --- */

/** Get DSS version. Returns (major << 8) | minor */
u16 dss_version(void) SPRINTER_NAKED_DECL;

/** Exit program with error code */
void dss_exit(u8 code) SPRINTER_NAKED_DECL;

/** Enable interrupts */
void dss_ei(void) SPRINTER_NAKED_DECL;

/** Disable interrupts */
void dss_di(void) SPRINTER_NAKED_DECL;

/** Get current disk number (0=A:, 1=B:, 2=C:, ...) */
u8 dss_getdisk(void) SPRINTER_NAKED_DECL;

/** Set current disk */
void dss_setdisk(u8 disk) SPRINTER_NAKED_DECL;

/* --- Console I/O --- */

/** Print a single character to console */
void dss_putchar(u8 ch) SPRINTER_NAKED_DECL;

/** Print a null-terminated string to console */
void dss_puts(const char *str) SPRINTER_NAKED_DECL;

/** Wait for a key press, return character code */
u8 dss_waitkey(void) SPRINTER_NAKED_DECL;

/** Wait for a key press and return full keyboard state */
void dss_waitkey_ex(dss_key_t *key) SPRINTER_NAKED_DECL;

/** Read and consume a key without waiting. Returns false if buffer is empty */
bool dss_scankey(dss_key_t *key) SPRINTER_NAKED_DECL;

/** Inspect next buffered key without consuming it. Returns false if buffer is empty */
bool dss_testkey(dss_key_t *key) SPRINTER_NAKED_DECL;

/** Check if a key is available in keyboard buffer */
bool dss_kbhit(void) SPRINTER_NAKED_DECL;

/** Get key with echo */
u16 dss_getche(void) SPRINTER_NAKED_DECL;

/** Set cursor position (1-based) */
void dss_gotoxy(u8 x, u8 y) SPRINTER_NAKED_DECL;

/** Clear screen */
void dss_clrscr(void) SPRINTER_NAKED_DECL;

/* --- File I/O --- */

/** Open file, returns file descriptor or -1 on error */
i16 dss_open(const char *path, u8 mode) SPRINTER_NAKED_DECL;

/** Create new file, returns file descriptor or -1 on error */
i16 dss_creat(const char *path) SPRINTER_NAKED_DECL;

/** Close file descriptor */
u8 dss_close(u8 fd) SPRINTER_NAKED_DECL;

/** Read from file. Returns bytes actually read, or -1 on error */
i16 dss_read(u8 fd, void *buf, u16 count) SPRINTER_NAKED_DECL;

/** Write to file. Returns bytes actually written, or -1 on error */
i16 dss_write(u8 fd, const void *buf, u16 count) SPRINTER_NAKED_DECL;

/** Seek in file. Returns 0 on success, -1 on error */
i16 dss_seek(u8 fd, u32 offset, u8 origin) SPRINTER_NAKED_DECL;

/** Delete file. Returns 0 on success, error code otherwise */
u8 dss_delete(const char *path) SPRINTER_NAKED_DECL;

/** Rename file. Returns 0 on success */
u8 dss_rename(const char *oldpath, const char *newpath) SPRINTER_NAKED_DECL;

/* --- Directories --- */

/** Change directory. Returns 0 on success */
u8 dss_chdir(const char *path) SPRINTER_NAKED_DECL;

/** Get current directory path into buf. Returns 0 on success */
u8 dss_curdir(char *buf) SPRINTER_NAKED_DECL;

/** Create directory. Returns 0 on success */
u8 dss_mkdir(const char *path) SPRINTER_NAKED_DECL;

/** Remove directory. Returns 0 on success */
u8 dss_rmdir(const char *path) SPRINTER_NAKED_DECL;

/** Find first matching file */
i8 dss_ffirst(const char *pattern, dss_find_t *result, u8 attr) SPRINTER_NAKED_DECL;

/** Find next matching file */
i8 dss_fnext(dss_find_t *result) SPRINTER_NAKED_DECL;

/* --- Date/Time --- */

/** Get system date */
void dss_getdate(dss_date_t *d) SPRINTER_NAKED_DECL;

/** Get system time */
void dss_gettime(dss_time_t *t) SPRINTER_NAKED_DECL;

/** Set system date and time (both required in single call) */
void dss_settime(dss_date_t *d, dss_time_t *t) SPRINTER_NAKED_DECL;

/* --- Memory --- */

/** Set memory window page.
 *  win: 0-3 (memory window number)
 *  page: RAM page number
 */
void dss_setwin(u8 win, u8 page) SPRINTER_NAKED_DECL;

/** Allocate a memory page. Returns page/block id, or 0xFF if DSS GETMEM reports an error */
u8 dss_getmem(void) SPRINTER_NAKED_DECL;

/** Free a memory page */
void dss_freemem(u8 page) SPRINTER_NAKED_DECL;

/** Get memory info: total and free page counts */
void dss_meminfo(u16 *total, u16 *free_pages) SPRINTER_NAKED_DECL;

/* --- Video --- */

/** Set video mode. Returns 0 on success.
 *  mode: see VMODE_* constants in video.h
 *  page: 0 or 1
 */
u8 dss_setvmod(u8 mode, u8 page) SPRINTER_NAKED_DECL;

/** Get current video mode and page */
void dss_getvmod(u8 *mode, u8 *page) SPRINTER_NAKED_DECL;

/** Scroll screen area.
 *  dir: SCROLL_UP (1) or SCROLL_DOWN (2)
 *  count: number of lines to scroll (0 = clear line)
 */
void dss_scroll(u8 x, u8 y, u8 w, u8 h, u8 dir, u8 count) SPRINTER_NAKED_DECL;

/** Clear screen area with color and attribute */
void dss_clear(u8 x, u8 y, u8 w, u8 h, u8 color, u8 attr) SPRINTER_NAKED_DECL;

/* --- Process --- */

/** Execute a program. Returns exit code or -1 on error */
i16 dss_exec(const char *path) SPRINTER_NAKED_DECL;

/** Execute a program with raw DSS error capture.
 *  On error returns -1 and stores DSS error code from A into *err (if err != 0).
 *  On success returns child exit code and stores 0 into *err (if err != 0).
 */
i16 dss_exec_ex(const char *path, u8 *err) SPRINTER_NAKED_DECL;

/** Get exit code of the last EXEC'ed child from DSS ERLEVEL */
u8 dss_wait(void) SPRINTER_NAKED_DECL;

/** Get pointer to saved command line */
char *dss_cmdline(void);

/** Call code at given address. Saves/restores IX (SDCC frame pointer).
 *  Returns value left in HL by called code.
 */
u16 dss_call(u16 addr) SPRINTER_NAKED_DECL;

/** Call code at given address with parameter pushed on stack.
 *  Called code can read param via: pop hl (ret addr), pop de (param), push hl.
 *  Returns value left in HL by called code.
 */
u16 dss_callp(u16 addr, u16 param) SPRINTER_NAKED_DECL;

/** Parse path into components.
 *  subfunc: EXPATH_DRIVE, EXPATH_PATH, EXPATH_NAME, EXPATH_EXT, or EXPATH_ALL
 *  Returns 0 on success, -1 on error.
 */
i8 dss_expath(const char *path, char *buf, u8 subfunc) SPRINTER_NAKED_DECL;

/** Get application info.
 *  subfunc: APPINFO_PARAMS (0), APPINFO_DIR (1), or APPINFO_FULL (2)
 *  Returns 0 on success, -1 on error.
 */
i8 dss_appinfo(u8 subfunc, char *buf) SPRINTER_NAKED_DECL;

/* --- Environment --- */

/** Get environment variable value. Returns 0 on success, -1 if not found */
i8 dss_getenv(const char *name, char *buf) SPRINTER_NAKED_DECL;

/** Set environment variable. name_value format: "NAME=VALUE"
 *  Returns 0 on success, -1 on error.
 */
i8 dss_setenv(const char *namevalue) SPRINTER_NAKED_DECL;

#endif /* _SPRINTER_DSS_H */
