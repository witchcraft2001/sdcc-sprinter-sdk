# Sprinter Hardware API

The Sprinter-specific API provides direct access to DSS OS calls, BIOS functions, video hardware, the mouse driver, and low-level I/O ports. These headers are independent from the standard C library and are useful when you want smaller binaries or tighter control over the machine.

Include everything at once:

```c
#include <sprinter.h>
```

Or include only the parts you need:

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
| `bool` | 1 byte | `true` or `false` |

The header also provides `size_t`, `NULL`, and `offsetof` via `<stddef.h>`.

## sprinter/dss.h -- DSS OS Calls

All DSS system calls go through `RST #10`. The SDK wraps them in C functions with an SDCC 2.9.0-compatible ABI.

### Console and Keyboard

```c
void dss_putchar(u8 ch);
void dss_puts(const char *str);
u8   dss_waitkey(void);
void dss_waitkey_ex(dss_key_t *key);
bool dss_scankey(dss_key_t *key);
bool dss_testkey(dss_key_t *key);
bool dss_kbhit(void);
u16  dss_getche(void);
void dss_gotoxy(u8 x, u8 y);
void dss_clrscr(void);
```

- `dss_putchar(ch)` prints one character.
- `dss_puts(str)` prints a zero-terminated string exactly as provided. Use `\r\n` for a newline; DSS does not auto-translate `\n`.
- `dss_waitkey()` blocks until a key is pressed and returns the ASCII code.
- `dss_waitkey_ex(key)` blocks and fills a full keyboard-event structure.
- `dss_scankey(key)` is non-blocking; it returns `true` and consumes a key if one is waiting.
- `dss_testkey(key)` is also non-blocking, but does not consume the buffered key.
- `dss_kbhit()` is a fast yes/no keyboard-buffer check.
- `dss_getche()` reads a key and echoes it through DSS.
- `dss_gotoxy(x, y)` moves the text cursor. Coordinates are 1-based.
- `dss_clrscr()` clears the active text screen.

`dss_key_t` describes one keyboard event:

```c
typedef struct {
    u8 ascii;
    u8 scan;
    u8 modifiers;
    u8 locks;
} dss_key_t;
```

- `ascii` is the translated character, if any.
- `scan` is the physical/scan position code.
- `modifiers` contains Shift/Ctrl/Alt state.
- `locks` contains Caps/Num/Scroll/Insert/RusLat state.

Useful bit masks:

```c
DSS_KEYMOD_ALT
DSS_KEYMOD_CTRL
DSS_KEYMOD_LSHIFT
DSS_KEYMOD_RSHIFT
DSS_KEYLOCK_CAPS
DSS_KEYLOCK_NUM
DSS_KEYLOCK_SCROLL
DSS_KEYLOCK_RUSLAT
```

### File I/O

```c
i16  dss_open(const char *path, u8 mode);
i16  dss_creat(const char *path);
u8   dss_close(u8 fd);
i16  dss_read(u8 fd, void *buf, u16 count);
i16  dss_write(u8 fd, const void *buf, u16 count);
i16  dss_seek(u8 fd, u32 offset, u8 origin);
u8   dss_delete(const char *path);
u8   dss_rename(const char *oldpath, const char *newpath);
```

- `path` is a DSS path such as `"A:\\DIR\\FILE.TXT"` or `"FILE.TXT"`.
- `fd` is the 8-bit DSS file descriptor returned by `dss_open()` or `dss_creat()`.
- `buf` points to the read/write buffer.
- `count` is the number of bytes to transfer.
- `offset` is a 32-bit absolute or relative file offset.
- `origin` is one of `SEEK_SET`, `SEEK_CUR`, or `SEEK_END`.

Return values:

- `dss_open()` / `dss_creat()` return a descriptor or `-1`.
- `dss_read()` / `dss_write()` return the number of bytes transferred or `-1`.
- `dss_seek()` returns `0` on success or `-1` on error.
- `dss_delete()` / `dss_rename()` return `0` on success, or a DSS error code otherwise.

Open-mode bits can be combined with OR:

| Constant | Value | Description |
|----------|-------|-------------|
| `O_RDONLY` | 0x00 | Read only |
| `O_WRONLY` | 0x01 | Write only |
| `O_RDWR` | 0x02 | Read and write |
| `O_CREAT` | 0x04 | Create if missing |
| `O_TRUNC` | 0x08 | Truncate to zero length |
| `O_APPEND` | 0x10 | Append at end of file |

### Directories and Search

```c
u8   dss_chdir(const char *path);
u8   dss_curdir(char *buf);
u8   dss_mkdir(const char *path);
u8   dss_rmdir(const char *path);
i8   dss_ffirst(const char *pattern, dss_find_t *result, u8 attr);
i8   dss_fnext(dss_find_t *result);
```

- `dss_chdir(path)` changes the current directory.
- `dss_curdir(buf)` writes the current directory into `buf`. DSS returns the directory path without the drive prefix.
- `dss_mkdir(path)` creates one directory level.
- `dss_rmdir(path)` removes an empty directory.
- `dss_ffirst(pattern, result, attr)` starts a search such as `"*.C"` or `"*.*"`.
- `dss_fnext(result)` continues the same search using the state already stored in `result`.

`dss_find_t` stores both the search context and the matched file information:

```c
typedef struct {
    char     name[8];
    char     ext[3];
    u8       attrib;
    u8       reserved[10];
    u16      time;
    u16      date;
    u16      cluster;
    u16      size_lo;
    u16      size_hi;
    u8       attr;
    char     ff_name[223];
} dss_find_t;
```

- `ff_name` is the zero-terminated matched name.
- `attr` is the matched file attribute byte.
- `size_lo` and `size_hi` form a 32-bit file size.
- `date` and `time` use DOS-compatible packed formats.

Search attribute bits:

| Constant | Value | Description |
|----------|-------|-------------|
| `FA_NORMAL` | 0x00 | Normal files |
| `FA_RDONLY` | 0x01 | Read-only |
| `FA_HIDDEN` | 0x02 | Hidden |
| `FA_SYSTEM` | 0x04 | System |
| `FA_LABEL` | 0x08 | Volume label |
| `FA_DIREC` | 0x10 | Directory |
| `FA_ARCH` | 0x20 | Archive |

### Date and Time

```c
void dss_getdate(dss_date_t *d);
void dss_gettime(dss_time_t *t);
void dss_settime(dss_date_t *d, dss_time_t *t);
```

- `dss_getdate(d)` fills `year`, `month`, and `day`.
- `dss_gettime(t)` fills `hour`, `minute`, `second`, and `hundredths`.
- `dss_settime(d, t)` updates both date and time in one DSS call. Pass valid pointers for both structures.

Structures:

```c
typedef struct {
    u16 year;
    u8  day;
    u8  month;
} dss_date_t;

typedef struct {
    u8 minute;
    u8 hour;
    u8 hundredths;
    u8 second;
} dss_time_t;
```

### Memory Pages

```c
void dss_setwin(u8 win, u8 page);
u8   dss_getmem(void);
void dss_freemem(u8 page);
void dss_meminfo(u16 *total, u16 *free_pages);
```

- `dss_setwin(win, page)` maps a RAM page into memory window `0..3`.
- `dss_getmem()` allocates one RAM page and returns its page/block id. If DSS GETMEM reports an error (`CF=1`, `A=error code`), the SDK wrapper returns `0xFF`; callers must check this before using the value with `dss_setwin()` or `dss_freemem()`.
- `dss_freemem(page)` returns a page to DSS.
- `dss_meminfo(total, free_pages)` reports the total number of managed pages and how many are currently free.

### Process, Path, Environment, and DSS Text Helpers

```c
u16  dss_version(void);
void dss_exit(u8 code);
i16  dss_exec(const char *path);
i16  dss_exec_ex(const char *path, u8 *err);
u8   dss_wait(void);
char *dss_cmdline(void);
u16  dss_call(u16 addr);
u16  dss_callp(u16 addr, u16 param);
i8   dss_expath(const char *path, char *buf, u8 subfunc);
i8   dss_appinfo(u8 subfunc, char *buf);
i8   dss_getenv(const char *name, char *buf);
i8   dss_setenv(const char *namevalue);
u8   dss_getdisk(void);
void dss_setdisk(u8 disk);
void dss_ei(void);
void dss_di(void);
u8   dss_setvmod(u8 mode, u8 page);
void dss_getvmod(u8 *mode, u8 *page);
void dss_scroll(u8 x, u8 y, u8 w, u8 h, u8 dir, u8 count);
void dss_clear(u8 x, u8 y, u8 w, u8 h, u8 color, u8 attr);
```

- `dss_version()` returns the DSS version packed as `(major << 8) | minor`.
- `dss_exit(code)` terminates the current program and reports `code` to DSS.
- `dss_exec(path)` runs another program and returns its exit code, or `-1` if DSS could not start it.
- `dss_exec_ex(path, err)` is the same, but on launch failure it stores the raw DSS error code in `*err`.
- `dss_wait()` returns DSS `ERLEVEL`, which is the last child exit code recorded by DSS.
- `dss_cmdline()` returns a pointer to the saved command tail of the current process. Copy it if you need to keep it.
- `dss_call(addr)` calls machine code at `addr` and returns whatever that routine leaves in `HL`.
- `dss_callp(addr, param)` does the same, but pushes one 16-bit parameter for the callee.
- `dss_expath(path, buf, subfunc)` extracts one path component into `buf`.
- `dss_appinfo(subfunc, buf)` returns program metadata such as the original command tail or executable path.
- `dss_getenv(name, buf)` reads an environment variable into `buf`.
- `dss_setenv("NAME=VALUE")` sets or updates one environment variable.
- `dss_getdisk()` returns the current drive number (`0 = A:`, `1 = B:`, `2 = C:`...).
- `dss_setdisk(disk)` switches the current drive.
- `dss_ei()` / `dss_di()` enable or disable interrupts.
- `dss_setvmod(mode, page)` switches the DSS text/video mode and active page.
- `dss_getvmod(mode, page)` reads the current DSS mode and page.
- `dss_scroll(x, y, w, h, dir, count)` scrolls a rectangular text region.
- `dss_clear(x, y, w, h, color, attr)` clears a rectangular text region.

Path helper subfunctions:

| Constant | Meaning |
|----------|---------|
| `EXPATH_ALL` | Entire normalized path |
| `EXPATH_DRIVE` | Drive only, for example `C:` |
| `EXPATH_PATH` | Directory part |
| `EXPATH_NAME` | File name without extension |
| `EXPATH_EXT` | Extension without dot |

App-info subfunctions:

| Constant | Meaning |
|----------|---------|
| `APPINFO_PARAMS` | Command-line parameters only |
| `APPINFO_DIR` | Directory that contains the current executable |
| `APPINFO_FULL` | Full path of the current executable |

Scroll directions:

| Constant | Meaning |
|----------|---------|
| `SCROLL_UP` | Scroll region upward |
| `SCROLL_DOWN` | Scroll region downward |

`dss_clear()` note: the public prototype keeps the historical names `(color, attr)`, but the current DSS wrapper passes the fifth argument as the text attribute byte and the sixth as the fill character. In practice, calls look like `dss_clear(4, 8, 34, 6, 0x1F, ' ')`.

## dir.h -- DOS/Turbo C Compatibility Helpers

`<dir.h>` provides thin wrappers and aliases on top of the DSS API:

```c
char *getcwd(char *buf, int size);
int   fnsplit(const char *path, char *drive, char *dir, char *fname, char *ext);
```

- `getcwd(buf, size)` fills `buf` with the current directory and returns `buf` on success, or `NULL` on error. The current implementation ignores `size`, so provide a buffer large enough for the path.
- `fnsplit(path, drive, dir, fname, ext)` splits a path into separate components and returns a bitmask of present parts.

`fnsplit()` flags:

| Constant | Meaning |
|----------|---------|
| `DRIVE` | Drive part was found |
| `DIRECTORY` | Directory part was found |
| `FILENAME` | File name part was found |
| `EXTENSION` | Extension part was found |
| `WILDCARDS` | Reserved for DOS/Turbo C compatibility |

The same header also aliases `chdir`, `mkdir`, `rmdir`, `findfirst`, `findnext`, `ffirst`, and `fnext` to the DSS-backed implementations.

## stdlib.h -- Environment Helpers

The SDK standard library also exposes DSS-backed environment helpers:

```c
char *getenv(const char *name);
int   putenv(const char *namevalue);
```

- `getenv(name)` returns a pointer to an internal static buffer on success, or `NULL` if the variable does not exist.
- `putenv("NAME=VALUE")` forwards to `dss_setenv()` and returns `0` on success.

## sprinter/bios.h -- BIOS Calls

BIOS calls go through `RST #08` and provide lower-level hardware access than DSS.

```c
void bios_setpal(u8 index, u8 r, u8 g, u8 b);
void bios_putpixel(u16 x, u8 y, u8 color);
u16  bios_version(void);
u8   bios_board_id(void);
u8   inp(u16 port);
void outp(u16 port, u8 value);
```

- `bios_setpal(index, r, g, b)` updates one palette entry. `index` is `0..255`; each color component is `0..63`.
- `bios_putpixel(x, y, color)` writes one pixel in graphics mode.
- `bios_version()` returns the BIOS version in BCD form.
- `bios_board_id()` returns the board or Sprinter type identifier.
- `inp(port)` and `outp(port, value)` wrap the Z80 `IN` and `OUT` instructions.

`bios_putpixel()` leaves the hardware VRAM row selector active. After direct pixel plotting, call `video_safe_porty()` before doing normal memory accesses again.

## sprinter/video.h -- Video Modes

### Mode Constants

| Constant | Value | Description |
|----------|-------|-------------|
| `VMODE_TEXT40` | 0x02 | Text 40x32 |
| `VMODE_TEXT80` | 0x03 | Text 80x32 |
| `VMODE_ZX` | 0x03 | Alias for `VMODE_TEXT80` |
| `VMODE_320_16` | 0x80 | 320x256, 16 colors |
| `VMODE_320` | 0x81 | 320x256, 256 colors |
| `VMODE_640_16` | 0x82 | 640x256, 16 colors |
| `VMODE_640` | 0x82 | Alias for `VMODE_640_16` |

### Functions

```c
void video_setmode(u8 mode);
u8   video_getmode(void);
void video_swap(void);
void video_sync_enable(void);
void video_sync_disable(void);
void video_vsync(void);
void video_setpal(u8 index, u8 r, u8 g, u8 b);
void video_setpal_range(u8 first, u16 count, const video_rgb6_t *colors);
void video_setpal_range8(u8 first, u16 count, const video_rgb8_t *colors);
void video_setpal_graf(void);
void video_mapvram(u8 win, u8 page);
void video_safe_porty(void);
```

- `video_setmode(mode)` switches the hardware video mode directly.
  Switching to a text mode also disables the hardware sync source.
- `video_getmode()` returns the current hardware mode byte.
- `video_swap()` flips display pages for double buffering through RGMOD bit 0.
- `video_sync_enable()` enables the Sprinter hardware sync source used by `video_vsync()`.
- `video_sync_disable()` disables that sync source again.
- `video_vsync()` enables the hardware sync source through Sprinter port `#004E`, then waits for
  the `#FFFE` bit 5 low-to-high transition. If the sync bit is unavailable, it
  falls back to one interrupt wait instead of hanging. The sync source remains enabled for
  subsequent frames and is disabled again by `video_setmode(VMODE_TEXT*)` or
  `dss_exit()`.
- `video_setpal(index, r, g, b)` sets one palette entry using 8-bit RGB values (`0..255` each).
- `video_setpal_range(first, count, colors)` sets a range of colors using 6-bit RGB values (`0..63`). `count` may be up to 255.
- `video_setpal_range8(first, count, colors)` sets a range of colors using 8-bit RGB values (`0..255`) and scales them to the hardware range.
- `video_setpal_graf()` loads the built-in BIOS GRAF palette.
- `video_mapvram(win, page)` maps a VRAM page into memory window `0..3`. VRAM pages typically start at `0x50`.
- `video_safe_porty()` restores `PORT_Y` to a safe value after raw pixel access.

### Screen Size Constants

```c
#define SCREEN_W_320    320
#define SCREEN_H_320    256
#define SCREEN_W_640    640
#define SCREEN_H_640    256
#define TEXT_COLS        80
#define TEXT_ROWS        32
```

## sprinter/gfx.h -- Optional Graphics Library

`gfx.lib` is separate from the base `sprinter.lib` and is linked only when requested. It targets `320x256x256` mode and provides a shared layer for sprites, blits, background restore, and graphics primitives.

```c
#include <sprinter/gfx.h>

void gfx_draw_sprite8(u8 screen, u16 x, u8 y, const void *data, u8 flags);
void gfx_draw_sprite16(u8 screen, u16 x, u8 y, const void *data, u8 flags);
void gfx_draw_sprite24(u8 screen, u16 x, u8 y, const void *data, u8 flags);

void gfx_draw_pixel(u8 screen, u16 x, u8 y, u8 color, u8 flags);
void gfx_draw_hline(u8 screen, u16 x, u8 y, u16 width, u8 color, u8 flags);
void gfx_draw_vline(u8 screen, u16 x, u8 y, u8 height, u8 color, u8 flags);
void gfx_draw_line(u8 screen, u16 x0, u8 y0, u16 x1, u8 y1, u8 color, u8 flags);
void gfx_draw_line_thick(u8 screen, u16 x0, u8 y0, u16 x1, u8 y1,
                         u8 thickness, u8 color, u8 flags);
void gfx_draw_rect(u8 screen, u16 x, u8 y, u16 width, u8 height, u8 color, u8 flags);
void gfx_draw_rect_thick(u8 screen, u16 x, u8 y, u16 width, u8 height,
                         u8 thickness, u8 color, u8 flags);
void gfx_fill_rect(u8 screen, u16 x, u8 y, u16 width, u8 height, u8 color, u8 flags);
void gfx_draw_circle(u8 screen, u16 cx, u8 cy, u8 radius, u8 color, u8 flags);

void gfx_restore_rect(u8 screen, u16 x, u8 y, u8 width, u8 height);
void gfx_restore_sprite8(u8 screen, u16 x, u8 y);
void gfx_restore_sprite16(u8 screen, u16 x, u8 y);
void gfx_restore_sprite24(u8 screen, u16 x, u8 y);

void gfx_copy_rect(u8 dst_screen, u8 src_screen, u16 x, u8 y, u8 width, u8 height);
void gfx_blit_rect(u8 dst_screen, u16 dst_x, u8 dst_y,
                   u8 src_screen, u16 src_x, u8 src_y,
                   u8 width, u8 height);
void gfx_scroll_rect(u8 screen, u16 dst_x, u8 dst_y,
                     u16 src_x, u8 src_y, u8 width, u8 height);
void gfx_copy_screen(u8 dst_screen, u8 src_screen);
void gfx_flip(void);

i16 gfx_load_resource_pages(const char *path, u8 first_page, u8 page_count);
u8  gfx_draw_resource(u8 screen, u16 x, u8 y, u8 base_page,
                      const gfx_resource_t *resources, u8 id, u8 flags);
```

Key flags:
- `GFX_OPAQUE` -- normal copy.
- `GFX_MASKED` -- color `0xFF` is transparent.
- `GFX_VRAM_ONLY` -- draw to VRAM only, without updating the video DRAM mirror.

Primitive functions use palette indices from the current graphics palette and accept the same `screen` and `flags` arguments as sprite functions. For `gfx_draw_vline()` and `gfx_fill_rect()`, `height == 0` means 256 rows, matching the existing full-screen copy convention.

`gfx_restore_rect()` restores a VRAM area from the DRAM mirror of the same screen. This is the Sprinter hardware mechanism for erasing temporary sprites: draw the background normally, draw the sprite with `GFX_VRAM_ONLY`, then copy the background preserved in DRAM back to VRAM.

`gfx_copy_rect()` copies a rectangle between logical screens 0/1 at the same coordinates. It is not the shadow-restore helper; it copies between screens, while the source is read from that screen's DRAM mirror. `gfx_blit_rect()` copies from independent source coordinates to independent destination coordinates, including same-screen scrolling cases. `gfx_scroll_rect()` is a same-screen convenience wrapper around `gfx_blit_rect()`. Sprites that are 16 or 24 pixels wide, `gfx_restore_rect()`, `gfx_copy_rect()`, and `gfx_blit_rect()` use the Sprinter hardware accelerator. Primitive functions are implemented as clipped direct VRAM writes. To link a single example, set `EXTRA_LIBS=$(SDK_DIR)build/gfx.lib`; from the SDK root, build the archive with `make gfx`.

For rectangle copy helpers, `height == 0` means 256 rows. `width == 0` is passed to the accelerator as a 256-byte line. `gfx_blit_rect()` copies through an internal 256-byte line buffer, so horizontal overlap is safe; when source and destination are on the same screen and the destination starts below the source, it copies rows bottom-up for vertical scrolling.

## sprinter/mouse.h -- Mouse Driver

The mouse driver is accessed through `RST #30`.

```c
u8   mouse_init(void);
void mouse_show(void);
void mouse_hide(void);
void mouse_stat(mouse_state_t *state);
void mouse_setpos(u16 x, u16 y);
void mouse_xbound(u16 min_x, u16 max_x);
void mouse_ybound(u16 min_y, u16 max_y);
```

- `mouse_init()` returns `1` if the driver and hardware are present, otherwise `0`.
- `mouse_show()` / `mouse_hide()` control the DSS mouse cursor.
- `mouse_stat(state)` fills the structure with button state and coordinates.
- `mouse_setpos(x, y)` moves the cursor.
- `mouse_xbound(min_x, max_x)` and `mouse_ybound(min_y, max_y)` clamp motion to a region.

`mouse_state_t`:

```c
typedef struct {
    u8  buttons;
    u16 x;
    u16 y;
} mouse_state_t;
```

Button masks:

```c
#define MS_BTN_LEFT     0x01
#define MS_BTN_RIGHT    0x02
#define MS_BTN_MIDDLE   0x04
```

## sprinter/ports.h -- Hardware Port Definitions

Port constants for direct use with `inp()` and `outp()`:

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
| `PORT_RGMOD` | 0xC9 | Display page / double-buffer control |
| `PORT_PAL_ADDR` | 0x89 | Palette address |
| `PORT_PAL_DATA` | 0x89 | Palette data |
| `PORT_CBL_DIR` | 0x004E | CBL/Covox-Blaster control; 16-bit port, use `OUT (C),A` |

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
| `PORT_KEYB` | 0xFE | ZX keyboard and border |
| `PORT_KEMPSTON` | 0x1F | Kempston joystick |
| `PORT_MOUSE_BTN` | 0x58 | Mouse buttons |
| `PORT_CTC0`..`PORT_CTC3` | 0x10-0x13 | CTC timer channels |
| `PORT_SIO_A_DATA` | 0x18 | PS/2 data |
| `PORT_SIO_A_CMD` | 0x19 | PS/2 command |
| `VRAM_PAGE_BASE` | 0x50 | First VRAM page number |
