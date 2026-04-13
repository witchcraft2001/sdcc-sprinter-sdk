# Sprinter Hardware API

The Sprinter-specific API provides direct access to DSS OS calls, BIOS functions, video hardware, mouse driver, and I/O ports. These headers are independent of the standard C library and produce smaller binaries.

Include everything at once:

```c
#include <sprinter.h>
```

Or include individual headers as needed:

```c
#include <sprinter/dss.h>
#include <sprinter/bios.h>
#include <sprinter/video.h>
#include <sprinter/mouse.h>
#include <sprinter/ports.h>
#include <sprinter/types.h>
```

## sprinter/types.h -- Type Aliases

Fixed-width integer types for Z80/SDCC:

| Type | Size | Range |
|------|------|-------|
| `u8` / `uint8_t` | 1 byte | 0 .. 255 |
| `i8` / `int8_t` | 1 byte | -128 .. 127 |
| `u16` / `uint16_t` | 2 bytes | 0 .. 65535 |
| `i16` / `int16_t` | 2 bytes | -32768 .. 32767 |
| `u32` / `uint32_t` | 4 bytes | 0 .. 4294967295 |
| `i32` / `int32_t` | 4 bytes | -2147483648 .. 2147483647 |
| `byte` | 1 byte | Alias for `unsigned char` |
| `word` | 2 bytes | Alias for `unsigned int` |
| `bool` | 1 byte | `true` (1) or `false` (0) |

Also includes `size_t`, `NULL`, and `offsetof` from `<stddef.h>`.

## sprinter/dss.h -- DSS OS Calls

All DSS system calls go through the `RST #10` vector. The SDK wraps each call in a C function that saves/restores the IX register (DSS may clobber it).

### Console I/O

```c
void dss_putchar(u8 ch);              /* Print character */
void dss_puts(const char *str);        /* Print null-terminated string */
u8   dss_waitkey(void);               /* Wait for key, return char code */
bool dss_kbhit(void);                 /* Non-blocking key check */
u16  dss_getche(void);                /* Get key with echo */
void dss_gotoxy(u8 x, u8 y);         /* Set cursor position (1-based) */
void dss_clrscr(void);                /* Clear screen */
```

**Note:** `dss_puts()` outputs the string as-is. Use `\r\n` for newlines (DSS does not translate `\n`).

### File I/O

```c
i16  dss_open(const char *path, u8 mode);    /* Open file, returns fd or -1 */
i16  dss_creat(const char *path);            /* Create file, returns fd or -1 */
u8   dss_close(u8 fd);                       /* Close file */
i16  dss_read(u8 fd, void *buf, u16 count);  /* Read bytes, returns count or -1 */
i16  dss_write(u8 fd, const void *buf, u16 count);  /* Write bytes */
i16  dss_seek(u8 fd, u32 offset, u8 origin); /* Seek: SEEK_SET/CUR/END */
u8   dss_delete(const char *path);           /* Delete file */
u8   dss_rename(const char *old, const char *new);  /* Rename file */
```

**File open modes** (combinable with OR):

| Constant | Value | Description |
|----------|-------|-------------|
| `O_RDONLY` | 0x00 | Read only |
| `O_WRONLY` | 0x01 | Write only |
| `O_RDWR` | 0x02 | Read and write |
| `O_CREAT` | 0x04 | Create if not exists |
| `O_TRUNC` | 0x08 | Truncate to zero length |
| `O_APPEND` | 0x10 | Append mode |

**Example:**

```c
#include <sprinter.h>

void main(void) {
    i16 fd;
    char buf[32];

    fd = dss_creat("TEST.TXT");
    if (fd < 0) { dss_puts("Error!\r\n"); return; }
    dss_write((u8)fd, "Hello\r\n", 7);
    dss_close((u8)fd);

    fd = dss_open("TEST.TXT", O_RDONLY);
    if (fd >= 0) {
        i16 n = dss_read((u8)fd, buf, 31);
        buf[n] = 0;
        dss_puts(buf);
        dss_close((u8)fd);
    }
    dss_delete("TEST.TXT");
}
```

### Directory Operations

```c
u8   dss_chdir(const char *path);
i8   dss_ffirst(const char *pattern, dss_find_t *result, u8 attr);
i8   dss_fnext(dss_find_t *result);
```

**dss_find_t structure:**

```c
typedef struct {
    char     name[8];       /* Filename pattern */
    char     ext[3];        /* Extension pattern */
    u8       attrib;        /* Search attribute */
    u8       reserved[10];
    u16      time;          /* File time (DOS format) */
    u16      date;          /* File date (DOS format) */
    u16      cluster;       /* First cluster */
    u16      size_lo;       /* File size, low word */
    u16      size_hi;       /* File size, high word */
    u8       attr;          /* Matched file attribute */
    char     ff_name[223];  /* Matched filename (null-terminated) */
} dss_find_t;
```

**File attributes:**

| Constant | Value | Description |
|----------|-------|-------------|
| `FA_NORMAL` | 0x00 | Normal files |
| `FA_RDONLY` | 0x01 | Read-only |
| `FA_HIDDEN` | 0x02 | Hidden |
| `FA_SYSTEM` | 0x04 | System |
| `FA_LABEL` | 0x08 | Volume label |
| `FA_DIREC` | 0x10 | Directory |
| `FA_ARCH` | 0x20 | Archive |

**Example -- directory listing:**

```c
#include <stdio.h>
#include <sprinter/dss.h>

void main(void) {
    dss_find_t entry;
    if (dss_ffirst("*.*", &entry, 0x20) == 0) {
        do {
            printf("%s\n", entry.ff_name);
        } while (dss_fnext(&entry) == 0);
    }
}
```

### Date and Time

```c
void dss_getdate(dss_date_t *d);    /* Get system date */
void dss_gettime(dss_time_t *t);    /* Get system time */
```

**Structures:**

```c
typedef struct {
    u16  year;
    u8   day;
    u8   month;
} dss_date_t;

typedef struct {
    u8   minute;
    u8   hour;
    u8   hundredths;
    u8   second;
} dss_time_t;
```

### Memory Management

```c
void dss_setwin(u8 win, u8 page);   /* Map RAM page into window 0-3 */
u8   dss_getmem(void);              /* Allocate page, returns page# or 0xFF */
void dss_freemem(u8 page);          /* Free allocated page */
```

### Process Control

```c
void dss_exit(u8 code);             /* Exit program */
i16  dss_exec(const char *path);    /* Execute program, returns exit code */
char *dss_cmdline(void);            /* Get saved command line pointer */
```

### System

```c
u8   dss_getdisk(void);             /* Get current disk (0=A, 1=B, ...) */
void dss_setdisk(u8 disk);          /* Set current disk */
void dss_ei(void);                  /* Enable interrupts */
void dss_di(void);                  /* Disable interrupts */
```

## sprinter/bios.h -- BIOS Calls

BIOS calls go through the `RST #08` vector and provide low-level hardware access.

### Graphics

```c
void bios_setpal(u8 index, u8 r, u8 g, u8 b);
```

Set a single palette entry. `index` is 0-255. Color components `r`, `g`, `b` are 0-63 each (6-bit color).

```c
void bios_putpixel(u16 x, u8 y, u8 color);
```

Draw a pixel in graphics mode. `x` is 0-319 (in 320x256 mode), `y` is 0-255, `color` is the palette index.

**Important:** after drawing pixels, call `video_safe_porty()` to reset the PORT_Y register. See [Graphics Guide](06_graphics_guide.md) for details.

### System Information

```c
u16 bios_version(void);    /* Returns version in BCD (e.g., 0x0102 = v1.02) */
u8  bios_board_id(void);   /* Returns board/Sprinter type identifier */
```

### Port I/O

```c
u8   inp(u16 port);              /* Read from I/O port */
void outp(u16 port, u8 value);   /* Write to I/O port */
```

These wrap the Z80 `IN` and `OUT` instructions. Use with port constants from `<sprinter/ports.h>`.

## sprinter/video.h -- Video Modes

### Video Mode Constants

| Constant | Value | Description |
|----------|-------|-------------|
| `VMODE_TEXT40` | 0x02 | Text 40x32 |
| `VMODE_TEXT80` | 0x03 | Text 80x32 |
| `VMODE_ZX` | 0x03 | Alias for `VMODE_TEXT80` |
| `VMODE_320_16` | 0x80 | 320x256, 16 colors (4bpp) |
| `VMODE_320` | 0x81 | 320x256, 256 colors (8bpp) |
| `VMODE_640_16` | 0x82 | 640x256, 16 colors |
| `VMODE_640` | 0x82 | Alias for `VMODE_640_16` |

### Functions

```c
void video_setmode(u8 mode);     /* Set video mode */
u8   video_getmode(void);        /* Get current video mode */
void video_swap(void);           /* Swap double buffer (via RGMOD port) */
void video_vsync(void);          /* Wait for vertical sync */
void video_setpal(u8 index, u8 r, u8 g, u8 b);  /* Set palette (0-255 per component) */
void video_mapvram(u8 win, u8 page);  /* Map VRAM page into memory window */
void video_safe_porty(void);     /* Reset PORT_Y to safe zone */
```

**Critical:** `video_safe_porty()` must be called after finishing all pixel drawing operations. See [Graphics Guide](06_graphics_guide.md).

### Screen Size Constants

```c
#define SCREEN_W_320    320
#define SCREEN_H_320    256
#define SCREEN_W_640    640
#define SCREEN_H_640    256
#define TEXT_COLS        80
#define TEXT_ROWS        32
```

## sprinter/mouse.h -- Mouse Driver

The mouse driver is accessed through the `RST #30` vector.

### Functions

```c
u8   mouse_init(void);                     /* Init driver, returns 1 if present */
void mouse_show(void);                     /* Show cursor */
void mouse_hide(void);                     /* Hide cursor */
void mouse_stat(mouse_state_t *state);     /* Get position and buttons */
void mouse_setpos(u16 x, u16 y);          /* Set cursor position */
void mouse_xbound(u16 min_x, u16 max_x);  /* Set X boundaries */
void mouse_ybound(u16 min_y, u16 max_y);  /* Set Y boundaries */
```

### mouse_state_t Structure

```c
typedef struct {
    u8   buttons;    /* Button state bitmask */
    u16  x;          /* X position */
    u16  y;          /* Y position */
} mouse_state_t;
```

### Button Masks

```c
#define MS_BTN_LEFT     0x01
#define MS_BTN_RIGHT    0x02
#define MS_BTN_MIDDLE   0x04
```

**Example:**

```c
#include <sprinter.h>

void main(void) {
    mouse_state_t ms;

    if (!mouse_init()) {
        dss_puts("No mouse!\r\n");
        return;
    }

    mouse_xbound(0, 319);
    mouse_ybound(0, 255);
    mouse_show();

    while (1) {
        mouse_stat(&ms);
        if (ms.buttons & MS_BTN_RIGHT) break;
    }

    mouse_hide();
}
```

## sprinter/ports.h -- Hardware Port Definitions

Constants for direct I/O port access with `inp()` / `outp()`:

### Memory Window Selectors

| Constant | Port | Window |
|----------|------|--------|
| `PORT_WIN0` | 0x82 | 0x0000-0x3FFF |
| `PORT_WIN1` | 0xA2 | 0x4000-0x7FFF |
| `PORT_WIN2` | 0xC2 | 0x8000-0xBFFF |
| `PORT_WIN3` | 0xE2 | 0xC000-0xFFFF |

### Video

| Constant | Port | Description |
|----------|------|-------------|
| `PORT_GFXMODE` | 0xC3 | Graphics mode register |
| `PORT_RGMOD` | 0xC9 | Double buffer / screen control |
| `PORT_PAL_ADDR` | 0x89 | Palette address |
| `PORT_PAL_DATA` | 0x89 | Palette data |

### Sound

| Constant | Port | Description |
|----------|------|-------------|
| `PORT_AY_ADDR` | 0x8C | AY-3-8910 register address |
| `PORT_AY_WRITE` | 0x8D | AY-3-8910 data write |
| `PORT_AY_READ` | 0x8E | AY-3-8910 data read |
| `PORT_COVOX` | 0x88 | Covox DAC output |

### Other

| Constant | Port | Description |
|----------|------|-------------|
| `PORT_KEYB` | 0xFE | ZX keyboard + border |
| `PORT_KEMPSTON` | 0x1F | Kempston joystick |
| `PORT_MOUSE_BTN` | 0x58 | Mouse buttons |
| `PORT_CTC0`..`PORT_CTC3` | 0x10-0x13 | CTC timer channels |
| `PORT_SIO_A_DATA` | 0x18 | PS/2 data |
| `PORT_SIO_A_CMD` | 0x19 | PS/2 command |
| `VRAM_PAGE_BASE` | 0x50 | Base VRAM page number |
