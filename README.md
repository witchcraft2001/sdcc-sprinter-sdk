# SDCC Sprinter SDK

Cross-platform C development toolkit for the **ZX Sprinter** computer (Z80 CPU, 7/21 MHz) running **Estex DSS** OS.

## Features

- **Standard C library** — `stdio.h`, `stdlib.h`, `string.h`, `ctype.h`, `conio.h` for easy porting of existing programs
- **SDCC 2.9.0 toolchain** for Z80 — fixed target compiler for this SDK
- **sjasmplus 1.22** assembler — for mixed C+ASM projects
- **Sprinter hardware API** — DSS OS calls, BIOS, video modes, mouse driver, port I/O
- **Compatibility headers** — `io.h`, `dos.h`, `dir.h` for easy porting from Turbo C / SOLID C
- **Granular linking** — each function in a separate module; linker includes only what's used
- **19 example programs** with Makefiles — from "Hello World" to EXEC/error handling, APPINFO, ENVIRON and CALL demos
- **ihx2exe.py** tool — converts SDCC output to Sprinter `.EXE` format
- **ihx2bin.py** tool — converts SDCC output to raw binary (for overlays and resident code)
- **Raw binary support** — build code fragments for loading at arbitrary addresses
- Works on **macOS, Linux, Windows** (MSYS2/MinGW)

## Quick Start

### 1. Install Prerequisites

**macOS:**
```bash
brew install python3
```

**Ubuntu / Debian:**
```bash
sudo apt install python3 make
```

**Windows (MSYS2):**
1. Install [MSYS2](https://www.msys2.org/)
2. Open MSYS2 MinGW64 terminal:
```bash
pacman -S mingw-w64-x86_64-sdcc mingw-w64-x86_64-python3 make
```

**Windows (manual):**
1. Install [Python 3.x](https://www.python.org/downloads/) (add to PATH)
2. Install SDCC 2.9.0.
   The recommended setup is to point the SDK at the original SDCC 2.9.0 `bin` directory.
   That directory should contain `sdcc` or `sdcc-2.9.0`, `sdcpp-2.9.0`, and either the upstream tool names `as-z80-2.9.0`, `sdcclib-2.9.0`, `link-z80-2.9.0` or repackaged equivalents `sdasz80`, `sdar`, `sdldz80`.
3. Install [GNU Make](https://gnuwin32.sourceforge.net/packages/make.htm) or use MSYS2/Git Bash

**Verify installation:**
```bash
/absolute/path/to/sdcc-2.9.0/bin/sdcc --version
python3 --version # Should show 3.x
make --version    # GNU Make
```

If several SDCC versions are installed, the SDK can be pinned to a specific 2.9.0 toolchain directory:

```bash
make SDCC290_BIN_DIR=/absolute/path/to/sdcc-2.9.0/bin
make examples SDCC290_BIN_DIR=/absolute/path/to/sdcc-2.9.0/bin
```

Or create `config.local.mk` from `config.local.mk.example`:

```makefile
SDCC290_BIN_DIR := /absolute/path/to/sdcc-2.9.0/bin
```

If you prefer not to use `SDCC290_BIN_DIR`, you can pin each tool explicitly:

```makefile
SDCC    := /absolute/path/to/sdcc-2.9.0/bin/sdcc
SDCPP   := /absolute/path/to/sdcc-2.9.0/bin/sdcpp-2.9.0
SDASZ80 := /absolute/path/to/sdcc-2.9.0/bin/as-z80-2.9.0
SDAR    := /absolute/path/to/sdcc-2.9.0/bin/sdcclib-2.9.0
SDLDZ80 := /absolute/path/to/sdcc-2.9.0/bin/link-z80-2.9.0
```

### 2. Build the SDK

```bash
make
```

This compiles all library modules into `build/sprinter.lib` (archive with selective linking).

### 3. Build Examples

```bash
make examples
```

Examples that generate runtime resources can optionally pack those files with
the HRUST-compatible `mhmt` tool from the local `kode` tree:

```bash
make examples PACK_ASSETS=1
```

Packed files keep the same DOS 8.3 runtime names such as `GFXDEMO.GFX`,
`BALLS.GFX`, `PPONG.GFX`, and `PPONG.PT3`. Programs load them through the same
SDK resource loaders; plain, unpacked files remain supported.

### 4. Run on Sprinter

Copy `.exe` files to the Sprinter disk (real hardware or emulator) and run from DSS command line:
```
A:\>TEST\HELLO.EXE
```

### 5. Build sjasmplus (optional)

Only needed for mixed C+ASM projects:
```bash
make tools
```

## Examples

| # | Name | Size | Description |
|---|------|------|-------------|
| 01 | hello | 681 B | Minimal program — `dss_puts()` |
| 02 | files | 1.4 KB | DSS low-level file I/O (create/write/read/delete) |
| 03 | graphics | 853 B | 320x256 graphics mode with palette and pixel drawing |
| 04 | mouse | 1.3 KB | Mouse driver: init, cursor, position tracking |
| 05 | keyinput | 1.6 KB | System info (BIOS version, date/time) + keyboard echo with modifier state |
| 06 | printf | 2.8 KB | `printf()` / `puts()` / `getchar()` demo |
| 07 | fileio | 3.7 KB | Standard I/O: `fopen` / `fwrite` / `fread` / `fclose` / `remove` |
| 08 | sort | 4.3 KB | Sorting algorithms: Bubble, Shell, Quick sort (ported from SOLID C) |
| 09 | dirlist | 2.9 KB | Directory listing via `dss_ffirst` / `dss_fnext` |
| 10 | bin2c | 4.0 KB | Utility: converts binary file to C byte array (CLI args) |
| 11 | fprintf | 3.7 KB | `fprintf()` to file + `fgets()` readback |
| 12 | strings | 3.6 KB | `string.h` + `ctype.h` functions demo |
| 13 | random | 3.4 KB | `stdlib.h` demo: `rand()`, `srand()`, `atoi()`, `abs()` |
| 14 | bench | 4.7 KB | Cross-compiler benchmark (Sieve, Sort, CRC-16, RC4, etc.) |
| 15 | exec | ~3 KB | Run child `.EXE` via `dss_exec_ex()` and print exit/error code |

## Documentation

Full SDK documentation is available in the `docs/` directory:

- **[English](docs/en/)** — installation, standard library, Sprinter API, graphics, porting from SOLID C, memory & linking
- **[Русский](docs/ru/)** — установка, стандартная библиотека, API Sprinter, графика, портирование с SOLID C, память и линковка

Key chapters:
- [Porting from SOLID C](docs/en/07_porting_from_solidc.md) — header/function/type mapping, step-by-step migration guide
- [Standard Library Reference](docs/en/04_standard_library.md) — stdio.h, stdlib.h, string.h, ctype.h, conio.h
- [Graphics Guide](docs/en/06_graphics_guide.md) — 320x256 mode, palette, VRAM pixel drawing
- [Extended WIN0..WIN2 layout](docs/en/11_extended_layout.md) — ~47 KB programs (code in WIN0+WIN1, private WIN2) via RST trampolines, built with a one-line Makefile

## Creating Your Own Project

### Using Standard C (recommended for portability)

```c
#include <stdio.h>

void main(void) {
    printf("Hello, Sprinter!\n");
    getchar();
}
```

### Using Sprinter-specific API (for hardware access)

```c
#include <sprinter.h>

void main(void) {
    dss_puts("Hello, Sprinter!\r\n");
    dss_waitkey();
}
```

### Reading full keyboard state

`dss_waitkey()` is kept as a compact ANSI/Turbo C style helper when only the character code is needed. For DSS keyboard handling with modifiers and scan code, use `dss_key_t` with the extended APIs:

```c
#include <sprinter.h>

void main(void) {
    dss_key_t key;

    dss_waitkey_ex(&key);

    if ((key.modifiers & DSS_KEYMOD_CTRL) && (key.ascii == 'X' || key.ascii == 'x')) {
        dss_puts("Ctrl+X\r\n");
    }
}
```

### Project Makefile

Create a `Makefile` in your project directory:

```makefile
APP      = myapp
SRCS     = main.c
SDK_DIR  = /path/to/sdcc-sprinter-sdk/
include $(SDK_DIR)examples/common.mk
```

Then run `make` to build `myapp.exe`.

If the machine has multiple SDCC versions installed, pin the desired 2.9.0 toolchain with `config.local.mk` or pass `SDCC290_BIN_DIR=...` on the `make` command line.
Recommended value:
`/path/to/sdcc-2.9.0/bin`

For multiple source files:
```makefile
APP      = myapp
SRCS     = main.c utils.c
SDK_DIR  = /path/to/sdcc-sprinter-sdk/
include $(SDK_DIR)examples/common.mk
```

### Memory Layout Configuration

By default, programs load at `0x4100` (like SOLID C), giving ~32 KB for code+data across WIN1+WIN2. You can customize the memory layout by setting variables **before** the `include`:

```makefile
# Default layout (~32 KB code+data):
#   CODE_LOC = 0x4100   (WIN1+WIN2: 0x4100-0xBFFF)
#   STACK    = 0xBFFF

# Compact layout (~16 KB, code in WIN2 only):
CODE_LOC = 0x8100
STACK    = 0xBFFF

APP      = myapp
SRCS     = main.c
SDK_DIR  = /path/to/sdcc-sprinter-sdk/
include $(SDK_DIR)examples/common.mk
```

| Layout | CODE_LOC | STACK | Code+Data space |
|--------|----------|-------|-----------------|
| Default (SOLID C compatible) | `0x4100` | `0xBFFF` | ~32 KB (WIN1+WIN2) |
| Compact | `0x8100` | `0xBFFF` | ~16 KB (WIN2 only) |
| Extended | `0x4100` | `0xFFFE` | ~32 KB code + WIN3 for stack |
| Page2 (`CRT0_PAGE2=1`) | `0x4200` | `0x40FF` (bootstrap) | code in WIN1; CRT0 allocates WIN2 page at runtime, working stack at `0xBFFF` |
| Extended WIN0..WIN2 (`include ../win0.mk`) | `0x0180` | `0xBF00` | **~47 KB** (code WIN0+WIN1, private WIN2 for data/stack) |

For programs that need more than the standard 32 KB, the **extended WIN0..WIN2 layout** runs the program from WIN0 and routes BIOS/DSS/IM1 through trampolines (a small stage-1 loader streams the code into freshly allocated pages). It gives ~47 KB, and ordinary C — including `printf` and stdio — builds unchanged with a one-line Makefile:

```makefile
APP  = myapp
SRCS = main.c
include ../win0.mk
```

See [docs/en/11_extended_layout.md](docs/en/11_extended_layout.md) for the design, runtime files (`lib/win0/`), tools, and examples (`examples/35`, `37`–`42`).

`CRT0_PAGE2=1` selects an alternative CRT0 (`lib/crt0_page2.s`) that calls `DSS.GETMEM` + `DSS.SETWIN2` before `main()`, so WIN2 is a runtime-allocated page rather than part of the EXE image. Use it when **(a)** you want to keep the on-disk EXE small (code + initialised data fit in WIN1, ≤ ~15.5 KB), or **(b)** the program needs a contiguous 32 KB workspace at `0x8000-0xFFFF` (WIN2+WIN3) free of code and initialised data. To enable, add a single line to your project Makefile **before** `include`:

```makefile
APP        = small
SRCS       = main.c
SDK_DIR    = ../../
CRT0_PAGE2 = 1
include $(SDK_DIR)examples/common.mk
```

See [docs/en/08_memory_and_linking.md](docs/en/08_memory_and_linking.md) for the full layout, examples, and remap notes.

DSS allocates 3 memory pages for each program (WIN1, WIN2, WIN3), so all addresses in `0x4000-0xFFFF` are valid. The Sprinter has 4 MB RAM total — for programs needing more than 32 KB, use `dss_getmem()` and `dss_setwin()` for page banking.

## Standard C Library

The SDK provides standard C headers for easy porting of existing programs. Functions are linked selectively — only used functions are included in the final binary.

### `<stdio.h>` — Standard I/O

| Function | Description |
|----------|-------------|
| `printf(fmt, ...)` | Formatted output (%d, %u, %x, %X, %s, %c, %%) |
| `sprintf(buf, fmt, ...)` | Format to string buffer |
| `fprintf(fp, fmt, ...)` | Format to file stream |
| `puts(s)` | Print string + newline |
| `putchar(c)` | Print single character |
| `getchar()` | Read character from console |
| `fopen(path, mode)` | Open file ("r", "w", "a", "r+", "w+") |
| `fclose(fp)` | Close file |
| `fread(buf, size, count, fp)` | Binary read |
| `fwrite(buf, size, count, fp)` | Binary write |
| `fgetc(fp)` / `fputc(c, fp)` | Character I/O |
| `fgets(buf, size, fp)` | Read line |
| `fputs(s, fp)` | Write string to stream |
| `fseek(fp, offset, whence)` | Seek in file |
| `feof(fp)` / `ferror(fp)` | Status checks |
| `remove(path)` | Delete file |
| `stdin` / `stdout` / `stderr` | Standard streams |

**printf format support:** `%d`, `%i`, `%u`, `%x`, `%X`, `%s`, `%c`, `%%`, field width, zero-padding, left-align. The `%l` modifier is accepted (for forward compatibility) but integer conversions are 16-bit. No floating point.

### `<string.h>` — String Operations

| Function | Description |
|----------|-------------|
| `strlen`, `strcpy`, `strncpy` | Length, copy |
| `strcmp`, `strncmp` | Compare |
| `strcat`, `strncat` | Concatenate |
| `strchr`, `strrchr` | Find character |
| `memcpy`, `memset`, `memcmp` | Memory operations |

### `<ctype.h>` — Character Classification

`isalpha`, `isdigit`, `isalnum`, `isspace`, `isupper`, `islower`, `isprint`, `isxdigit`, `iscntrl`, `ispunct`, `toupper`, `tolower`

### `<stdlib.h>` — Utility Functions

| Function | Description |
|----------|-------------|
| `exit(code)` | Exit program |
| `atoi(s)` | String to integer |
| `abs(n)` | Absolute value |
| `rand()` / `srand(seed)` | Pseudo-random numbers |
| `getenv(name)` | Get environment variable (returns `char*`) |
| `putenv(s)` | Set environment variable (`"NAME=VALUE"`) |

### `<conio.h>` — Console I/O (CP/M / Turbo C compatible)

| Function | Description |
|----------|-------------|
| `getch()` | Read key (no echo) |
| `getche()` | Read key (with echo) |
| `kbhit()` | Check if key pressed |
| `putch(c)` | Output character |
| `cputs(s)` | Output string |
| `cprintf(fmt, ...)` | Formatted console output |
| `clrscr()` | Clear screen |
| `gotoxy(x, y)` | Set cursor position |

### Compatibility Headers (Turbo C / SOLID C)

These headers provide zero-overhead macro aliases for easy porting:

**`<io.h>`** — Low-level file I/O: `open`, `close`, `read`, `write`, `lseek`

**`<dos.h>`** — DOS interface: `getdate`, `gettime`, `enable`, `disable`, `getdisk`, `setdisk`, `struct date`, `struct time`

**`<dir.h>`** — Directory operations: `mkdir`, `rmdir`, `chdir`, `getcwd`, `getcurdir`, `findfirst`, `findnext`, `fnsplit`

## Sprinter Hardware API

For direct hardware access, use the Sprinter-specific headers. These are independent of the standard library.

### `<sprinter/dss.h>` — DSS OS Calls

| Function | Description |
|----------|-------------|
| `dss_puts(str)` | Print string |
| `dss_putchar(ch)` | Print character |
| `dss_waitkey()` | Wait for key, return ASCII only |
| `dss_waitkey_ex(&key)` | Wait for key, return ASCII + scan + modifiers + locks |
| `dss_scankey(&key)` | Non-blocking read with full keyboard state |
| `dss_testkey(&key)` | Non-destructive buffer check with full keyboard state |
| `dss_kbhit()` | Check keyboard |
| `dss_clrscr()` | Clear screen |
| `dss_gotoxy(x, y)` | Set cursor (1-based) |
| `dss_open(path, mode)` | Open file (returns fd or -1) |
| `dss_creat(path)` | Create file |
| `dss_close(fd)` | Close file |
| `dss_read(fd, buf, n)` | Read from file |
| `dss_write(fd, buf, n)` | Write to file |
| `dss_seek(fd, off, whence)` | Seek in file |
| `dss_delete(path)` | Delete file |
| `dss_rename(old, new)` | Rename file |
| `dss_chdir(path)` | Change directory |
| `dss_ffirst(pat, res, attr)` | Find first file |
| `dss_fnext(res)` | Find next file |
| `dss_getdate(d)` / `dss_gettime(t)` | Get date/time |
| `dss_settime(d, t)` | Set system date and time |
| `dss_exec(path)` | Run program |
| `dss_exit(code)` | Exit program |
| `dss_cmdline()` | Get command line arguments |
| `dss_setwin(win, page)` | Map memory page |
| `dss_getmem()` / `dss_freemem(page)` | Allocate/free memory page |
| `dss_meminfo(&total, &free)` | Get memory info (total/free pages) |
| `dss_ei()` / `dss_di()` | Enable/disable interrupts |
| `dss_version()` | Get DSS version |
| `dss_mkdir(path)` / `dss_rmdir(path)` | Create/remove directory |
| `dss_curdir(buf)` | Get current directory |
| `dss_setvmod(mode, page)` | Set video mode |
| `dss_getvmod(&mode, &page)` | Get video mode |
| `dss_scroll(x,y,w,h,dir,n)` | Scroll screen area |
| `dss_clear(x,y,w,h,clr,attr)` | Clear screen area |
| `dss_getenv(name, buf)` | Get environment variable |
| `dss_setenv(namevalue)` | Set environment variable |
| `dss_appinfo(subfunc, buf)` | Get application info (path, params) |
| `dss_expath(path, buf, sub)` | Parse path into components |
| `dss_call(addr)` | Call code at address (saves IX) |
| `dss_callp(addr, param)` | Call code at address with parameter |

### `<sprinter/bios.h>` — BIOS & Hardware

| Function | Description |
|----------|-------------|
| `bios_setpal(idx, r, g, b)` | Set palette entry (0-255 per channel, full 8-bit) |
| `bios_putpixel(x, y, color)` | Draw pixel (320x256 mode) |
| `bios_version()` | Get BIOS version |
| `bios_board_id()` | Get board ID |
| `inp(port)` | Read I/O port |
| `outp(port, value)` | Write I/O port |

### `<sprinter/video.h>` — Video Modes

| Function | Description |
|----------|-------------|
| `video_setmode(mode)` | Set mode: `VMODE_320` (320x256), `VMODE_ZX` (text) |
| `video_getmode()` | Get current mode |
| `video_swap()` | Swap double buffer |
| `video_vsync()` | Wait for VSync |
| `video_setpal(idx, r, g, b)` | Set palette color (0-255 per channel) |
| `video_safe_porty()` | Reset PORT_Y to safe zone (call after drawing!) |

### `<sprinter/mouse.h>` — Mouse Driver

| Function | Description |
|----------|-------------|
| `mouse_init()` | Init driver (returns 1 if present) |
| `mouse_show()` / `mouse_hide()` | Show/hide cursor |
| `mouse_stat(state)` | Get position & buttons |
| `mouse_setpos(x, y)` | Set cursor position |
| `mouse_xbound(min, max)` | Set X boundaries |
| `mouse_ybound(min, max)` | Set Y boundaries |

## Building Raw Binaries

For resident programs, overlays, or code fragments that run at arbitrary addresses, use `ihx2bin.py` and `common_bin.mk`:

```makefile
APP      = overlay
SRCS     = overlay.c
CODE_LOC = 0xC000
include $(SDK_DIR)examples/common_bin.mk
```

This produces a raw `.bin` file (no EXE header, no CRT0) ready to load at the specified address.

### Loading and executing at runtime

```c
#include <sprinter.h>

void main(void) {
    u8 page = dss_getmem();              /* allocate RAM page */
    dss_setwin(3, page);                 /* map to WIN3 (0xC000) */

    /* Load binary into mapped memory */
    i16 fd = dss_open("OVERLAY.BIN", O_RDONLY);
    dss_read((u8)fd, (void*)0xC000, 4096);
    dss_close((u8)fd);

    u16 result = dss_call(0xC000);       /* execute code */

    dss_freemem(page);                   /* free page */
}
```

`dss_call(addr)` saves/restores IX (SDCC frame pointer). `dss_callp(addr, param)` passes a parameter on the stack.

### Manual conversion

```bash
python3 tools/ihx2bin.py output.ihx output.bin --org 0xC000
```

## Memory Map

DSS allocates 3 pages (WIN1, WIN2, WIN3) for each program. Default layout (`CODE_LOC=0x4100`):

| Address Range | Window | Usage |
|---------------|--------|-------|
| `0x0000-0x3FFF` | WIN0 | System / available for banking |
| `0x4080-0x40FF` | WIN1 | Command line arguments (load - 0x80) |
| `0x4100-0x7FFF` | WIN1 | **Code + data** (start of program) |
| `0x8000-0xBFFF` | WIN2 | **Code + data** (continued) + stack |
| `0xC000-0xFFFF` | WIN3 | Available for VRAM mapping or data |

Stack grows downward from `0xBFFF`. Total code+data space: **~32 KB**.

Compact layout (`CODE_LOC=0x8100`):

| Address Range | Window | Usage |
|---------------|--------|-------|
| `0x4000-0x7FFF` | WIN1 | Available for data |
| `0x8100-0xBFFF` | WIN2 | **Code + data** + stack (~16 KB) |

## Project Structure

```
sdcc-sprinter-sdk/
├── Makefile                # Build system
├── README.md
├── include/
│   ├── stdio.h             # Standard C I/O
│   ├── stdlib.h            # Standard utilities
│   ├── string.h            # String operations
│   ├── ctype.h             # Character classification
│   ├── conio.h             # Console I/O (CP/M compatible)
│   ├── io.h                # Low-level I/O (Turbo C / SOLID C compat)
│   ├── dos.h               # DOS interface (Turbo C / SOLID C compat)
│   ├── dir.h               # Directory ops (Turbo C / SOLID C compat)
│   ├── stddef.h            # size_t, NULL
│   ├── stdbool.h           # bool, true, false
│   ├── sprinter.h          # Umbrella header (all Sprinter APIs)
│   └── sprinter/
│       ├── types.h         # Integer types (u8, u16, etc.)
│       ├── ports.h         # Hardware I/O port definitions
│       ├── dss.h           # DSS OS API
│       ├── bios.h          # BIOS API
│       ├── video.h         # Video mode control
│       └── mouse.h         # Mouse driver
├── lib/
│   ├── crt0.s              # C runtime startup (default layout)
│   ├── crt0_page2.s        # CRT0 variant: allocates WIN2 page at startup
│   └── src/
│       ├── dss/            # DSS wrappers
│       ├── bios/           # BIOS wrappers (6 modules)
│       ├── video/          # Video functions (7 modules)
│       ├── mouse/          # Mouse functions (7 modules)
│       ├── stdio/          # Standard I/O (17 modules)
│       ├── stdlib/         # Standard library (5 modules)
│       ├── string/         # String functions (12 modules)
│       ├── ctype/          # Character functions (12 modules)
│       ├── conio/          # Console I/O (8 modules)
│       └── dir/            # Directory compat wrappers (2 modules)
├── tools/
│   ├── ihx2exe.py          # Intel HEX → Sprinter EXE converter
│   ├── ihx2bin.py          # Intel HEX → raw binary converter
│   ├── install-sdcc.sh     # SDCC installer script
│   └── build-sjasmplus.sh  # sjasmplus build script
├── examples/               # 19 example programs
│   ├── common.mk           # Shared build rules (EXE output)
│   ├── common_bin.mk       # Build rules for raw binary modules
│   ├── 01_hello/ .. 19_console/
├── build/
│   ├── crt0.rel            # Compiled CRT0
│   ├── crt0_page2.rel      # Compiled page2 CRT0
│   └── sprinter.lib        # Library archive
└── scripts/                # Local helper scripts (not part of the repository)
```

## Architecture Notes

### Selective Linking

Each library function is compiled into a separate `.rel` object file and packed into `sprinter.lib` using the SDCC librarian (`sdar` or `sdcclib-2.9.0`, depending on the toolchain package). The SDCC linker resolves only referenced symbols — unused functions add zero code to your binary.

Example: a program using only `puts("hello")` links ~170 bytes total. A program using `printf()` links ~2.8 KB (printf engine + fputc + dss_putchar).

### Calling Convention

The SDK targets the default ABI of SDCC 2.9.0:
- most function arguments are passed on the stack
- scalar return values come back in `L` / `HL`
- wrapper code preserves the caller context expected by SDCC 2.9.0, especially `IX`

### DSS System Calls

All DSS calls go through `RST #10` with function number in C register. IX is saved/restored around every call (DSS may clobber it).

DSS leaves the caller's return address intact on the stack across `RST #10`, so the wrappers (e.g. `dss_chdir.s`, `dss_call.s`) just `ret` after the call. This applies to `dss_exec` / `dss_exec_ex` too: even after a successful `EXEC`, DSS restores the parent SP so the caller's return address is exactly where it was before the call. Earlier versions of these two wrappers cached the return address and re-pushed it on return — that double-counted it and silently leaked 2 stack bytes per successful exec, which surfaced as crashes after sequential exec calls (e.g. inside `make`). Both wrappers now use the same plain-`ret` convention as the rest of the DSS bindings.

## Based On

- **SOLID C** library (2004) — original C library for Sprinter with DSS/BIOS wrappers
- **zx-sprinter-sdk** — game development SDK reference
- **Estex DSS** source code and documentation
- **ZX Sprinter BIOS** source code
- **SDCC** — Small Device C Compiler (https://sdcc.sourceforge.net/)
- **sjasmplus** — Z80 cross-assembler (https://github.com/z00m128/sjasmplus)

## License

SDK source code and examples are provided for free use in ZX Sprinter development.
