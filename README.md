# SDCC Sprinter SDK

Cross-platform C development toolkit for the **ZX Sprinter** computer (Z80 CPU, 7/21 MHz) running **Estex DSS** OS.

## Features

- **Standard C library** — `stdio.h`, `stdlib.h`, `string.h`, `ctype.h`, `conio.h` for easy porting of existing programs
- **SDCC 4.x compiler** for Z80 — free, open-source, cross-platform (macOS/Linux/Windows)
- **sjasmplus 1.22** assembler — for mixed C+ASM projects
- **Sprinter hardware API** — DSS OS calls, BIOS, video modes, mouse driver, port I/O
- **Granular linking** — each function in a separate module; linker includes only what's used
- **13 example programs** with Makefiles — from "Hello World" to sorting algorithms and graphics
- **ihx2exe.py** tool — converts SDCC output to Sprinter `.EXE` format
- Works on **macOS, Linux, Windows** (MSYS2/MinGW)

## Quick Start

### 1. Install Prerequisites

**macOS:**
```bash
brew install sdcc python3
```

**Ubuntu / Debian:**
```bash
sudo apt install sdcc python3 make
```

**Windows (MSYS2):**
1. Install [MSYS2](https://www.msys2.org/)
2. Open MSYS2 MinGW64 terminal:
```bash
pacman -S mingw-w64-x86_64-sdcc mingw-w64-x86_64-python3 make
```

**Windows (manual):**
1. Install [Python 3.x](https://www.python.org/downloads/) (add to PATH)
2. Install [SDCC](https://sdcc.sourceforge.net/) (Windows installer, add to PATH)
3. Install [GNU Make](https://gnuwin32.sourceforge.net/packages/make.htm) or use MSYS2/Git Bash

**Verify installation:**
```bash
sdcc --version    # Should show 4.x
python3 --version # Should show 3.x
make --version    # GNU Make
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
| 05 | keyinput | 1.4 KB | System info (BIOS version, date/time) + keyboard echo |
| 06 | printf | 2.8 KB | `printf()` / `puts()` / `getchar()` demo |
| 07 | fileio | 3.7 KB | Standard I/O: `fopen` / `fwrite` / `fread` / `fclose` / `remove` |
| 08 | sort | 4.3 KB | Sorting algorithms: Bubble, Shell, Quick sort (ported from SOLID C) |
| 09 | dirlist | 2.9 KB | Directory listing via `dss_ffirst` / `dss_fnext` |
| 10 | bin2c | 3.6 KB | Utility: converts binary file to C byte array |
| 11 | fprintf | 3.6 KB | `fprintf()` to file + `fgets()` readback |
| 12 | strings | 3.6 KB | `string.h` + `ctype.h` functions demo |
| 13 | random | 3.3 KB | `stdlib.h` demo: `rand()`, `srand()`, `atoi()`, `abs()` |

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

### Project Makefile

Create a `Makefile` in your project directory:

```makefile
APP      = myapp
SRCS     = main.c
SDK_DIR  = /path/to/sdcc-sprinter-sdk/
include $(SDK_DIR)examples/common.mk
```

Then run `make` to build `myapp.exe`.

For multiple source files:
```makefile
APP      = myapp
SRCS     = main.c utils.c
SDK_DIR  = /path/to/sdcc-sprinter-sdk/
include $(SDK_DIR)examples/common.mk
```

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

### `<stdlib.h>` — Utility Functions

| Function | Description |
|----------|-------------|
| `exit(code)` | Exit program |
| `atoi(s)` | String to integer |
| `abs(n)` | Absolute value |
| `rand()` / `srand(seed)` | Pseudo-random numbers |

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

## Sprinter Hardware API

For direct hardware access, use the Sprinter-specific headers. These are independent of the standard library.

### `<sprinter/dss.h>` — DSS OS Calls

| Function | Description |
|----------|-------------|
| `dss_puts(str)` | Print string |
| `dss_putchar(ch)` | Print character |
| `dss_waitkey()` | Wait for key |
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
| `dss_getdate(d)` / `dss_gettime(t)` | Date/time |
| `dss_exec(path)` | Run program |
| `dss_exit(code)` | Exit program |
| `dss_setwin(win, page)` | Map memory page |
| `dss_getmem()` / `dss_freemem(page)` | Memory management |
| `dss_ei()` / `dss_di()` | Interrupts |

### `<sprinter/bios.h>` — BIOS & Hardware

| Function | Description |
|----------|-------------|
| `bios_setpal(idx, r, g, b)` | Set palette entry (0-63 per channel) |
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
| `video_setpal(idx, r, g, b)` | Set palette color |
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

## Memory Map

| Address Range | Usage |
|---------------|-------|
| `0x0000-0x3FFF` | WIN0 — Available for data |
| `0x4000-0x7FFF` | WIN1 — Data segment (`--data-loc 0x4000`) |
| `0x8100-0xBFFF` | WIN2 — Code + stack (stack grows down from `0xBFFF`) |
| `0xC000-0xFFFF` | WIN3 — VRAM mapping or extra data |

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
│   ├── crt0.s              # C runtime startup
│   └── src/
│       ├── dss/            # DSS wrappers (30 modules)
│       ├── bios/           # BIOS wrappers (6 modules)
│       ├── video/          # Video functions (7 modules)
│       ├── mouse/          # Mouse functions (7 modules)
│       ├── stdio/          # Standard I/O (17 modules)
│       ├── stdlib/         # Standard library (4 modules)
│       ├── string/         # String functions (12 modules)
│       ├── ctype/          # Character functions (12 modules)
│       └── conio/          # Console I/O (8 modules)
├── tools/
│   ├── ihx2exe.py          # Intel HEX → Sprinter EXE converter (Python 3)
│   ├── install-sdcc.sh     # SDCC installer script
│   └── build-sjasmplus.sh  # sjasmplus build script
├── examples/               # 13 example programs
│   ├── common.mk           # Shared build rules
│   ├── 01_hello/ .. 13_random/
├── build/
│   ├── crt0.rel            # Compiled CRT0
│   └── sprinter.lib        # Library archive (~103 modules)
└── scripts/
    ├── copy_exe.sh          # Copy EXE files to target directory
    └── make_floppy.sh       # Create FAT12 floppy image
```

## Architecture Notes

### Selective Linking

Each library function is compiled into a separate `.rel` object file and packed into `sprinter.lib` using `sdar`. The SDCC linker resolves only referenced symbols — unused functions add zero code to your binary.

Example: a program using only `puts("hello")` links ~170 bytes total. A program using `printf()` links ~2.8 KB (printf engine + fputc + dss_putchar).

### Calling Convention

The SDK uses SDCC's default `sdcccall(1)` (register-based):
- 1st param: `u8` → A, `u16/ptr` → HL
- 2nd param: `u8` → L (if 1st in A), `u16/ptr` → DE
- Return: `u8` → A, `u16/ptr` → DE
- Callee-saved: IX only

### DSS System Calls

All DSS calls go through `RST #10` with function number in C register. IX is saved/restored around every call (DSS may clobber it).

## Based On

- **SOLID C** library (2004) — original C library for Sprinter with DSS/BIOS wrappers
- **zx-sprinter-sdk** — game development SDK reference
- **Estex DSS** source code and documentation
- **ZX Sprinter BIOS** source code
- **SDCC** — Small Device C Compiler (https://sdcc.sourceforge.net/)
- **sjasmplus** — Z80 cross-assembler (https://github.com/z00m128/sjasmplus)

## License

SDK source code and examples are provided for free use in ZX Sprinter development.
