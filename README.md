# SDCC Sprinter SDK

Cross-platform C development toolkit for the **ZX Sprinter** computer (Z80 CPU) running **Estex DSS** OS.

## Features

- **SDCC compiler** (4.1.0+) for Z80 — free, open-source, cross-platform
- **sjasmplus** assembler (1.22.0) — built from source for macOS/Linux
- Complete **DSS OS API** wrappers: file I/O, console, keyboard, directories, memory
- **BIOS API** wrappers: video, palette, pixel drawing, I/O ports
- **Mouse driver** API
- **Video mode** control (ZX/320x256/640x256)
- `ihx2exe.py` tool to produce Sprinter `.EXE` files
- 5 example programs with Makefiles
- Works on **macOS, Linux, Windows** (MSYS2)

## Quick Start

### 1. Install Prerequisites

```bash
# macOS
brew install sdcc python3

# Ubuntu/Debian
sudo apt install sdcc python3

# Build sjasmplus (optional, for mixed C+ASM projects)
make tools
```

### 2. Build the SDK Library

```bash
make
```

### 3. Build Examples

```bash
make examples
```

This produces `.exe` files for each example:
- `examples/01_hello/hello.exe` — Hello World
- `examples/02_files/files.exe` — File I/O (create, write, read, delete)
- `examples/03_graphics/graphics.exe` — 320x256 graphics mode with palette
- `examples/04_mouse/mouse.exe` — Mouse input tracking
- `examples/05_keyinput/keyinput.exe` — System info + keyboard echo

### 4. Run on Sprinter

Copy the `.exe` file to the Sprinter's disk (real hardware or emulator) and run it from DSS command line.

## Creating Your Own Project

### Minimal Example

```c
#include <sprinter.h>

void main(void) {
    dss_puts("Hello, Sprinter!\r\n");
    dss_waitkey();
}
```

### Build Command

```bash
# Compile
sdcc -mz80 --no-std-crt0 --code-loc 0x8100 --data-loc 0x4000 \
    --max-allocs-per-node 5000 --opt-code-speed \
    -I/path/to/sdk/include \
    /path/to/sdk/build/crt0.rel \
    /path/to/sdk/build/dss.rel \
    /path/to/sdk/build/bios.rel \
    /path/to/sdk/build/mouse.rel \
    /path/to/sdk/build/video.rel \
    main.c -o main.ihx

# Convert to Sprinter EXE
python3 /path/to/sdk/tools/ihx2exe.py main.ihx main.exe
```

### Using the Example Makefile

Create a `Makefile` in your project directory:

```makefile
APP      = myprogram
SRCS     = main.c
SDK_DIR  = /path/to/sdcc-sprinter-sdk/
include $(SDK_DIR)examples/common.mk
```

Then just `make`.

## API Reference

### DSS (OS Calls) — `<sprinter/dss.h>`

| Function | Description |
|----------|-------------|
| `dss_puts(str)` | Print null-terminated string |
| `dss_putchar(ch)` | Print single character |
| `dss_waitkey()` | Wait for key, return char code |
| `dss_kbhit()` | Check if key available |
| `dss_getche()` | Get key with echo |
| `dss_clrscr()` | Clear screen |
| `dss_gotoxy(x, y)` | Set cursor position (1-based) |
| `dss_open(path, mode)` | Open file, returns fd or -1 |
| `dss_creat(path)` | Create file, returns fd or -1 |
| `dss_close(fd)` | Close file descriptor |
| `dss_read(fd, buf, count)` | Read from file |
| `dss_write(fd, buf, count)` | Write to file |
| `dss_seek(fd, offset, origin)` | Seek in file |
| `dss_delete(path)` | Delete file |
| `dss_rename(old, new)` | Rename file |
| `dss_chdir(path)` | Change directory |
| `dss_ffirst(pattern, result, attr)` | Find first file |
| `dss_fnext(result)` | Find next file |
| `dss_getdate(d)` | Get system date |
| `dss_gettime(t)` | Get system time |
| `dss_getdisk()` | Get current disk letter |
| `dss_setdisk(disk)` | Set current disk |
| `dss_exit(code)` | Exit program |
| `dss_exec(path)` | Execute program |
| `dss_setwin(win, page)` | Map memory page to window |
| `dss_getmem()` | Allocate memory page |
| `dss_freemem(page)` | Free memory page |
| `dss_ei()` / `dss_di()` | Enable/disable interrupts |

### BIOS — `<sprinter/bios.h>`

| Function | Description |
|----------|-------------|
| `bios_setpal(idx, r, g, b)` | Set palette (2-bit RGB) |
| `bios_putpixel(x, y, color)` | Draw pixel |
| `bios_version()` | Get BIOS version |
| `bios_board_id()` | Get board ID |
| `inp(port)` | Read I/O port |
| `outp(port, value)` | Write I/O port |

### Video — `<sprinter/video.h>`

| Function | Description |
|----------|-------------|
| `video_setmode(mode)` | Set video mode (VMODE_ZX/320/640) |
| `video_getmode()` | Get current video mode |
| `video_swap()` | Swap double buffer |
| `video_vsync()` | Wait for vertical sync |
| `video_setpal(idx, r, g, b)` | Set palette color |
| `video_mapvram(win, page)` | Map VRAM page to window |

### Mouse — `<sprinter/mouse.h>`

| Function | Description |
|----------|-------------|
| `mouse_init()` | Init driver, returns button count |
| `mouse_show()` / `mouse_hide()` | Show/hide cursor |
| `mouse_stat(state)` | Get position & buttons |
| `mouse_setpos(x, y)` | Set cursor position |
| `mouse_xbound(min, max)` | Set X boundaries |
| `mouse_ybound(min, max)` | Set Y boundaries |

## Memory Map

| Address Range | Usage |
|---------------|-------|
| `0x0000-0x3FFF` | WIN0 — Available for data |
| `0x4000-0x7FFF` | WIN1 — Data segment (--data-loc 0x4000) |
| `0x8000-0x80FF` | EXE header (not in memory) |
| `0x8100-0xBFFF` | WIN2 — Code + stack |
| `0xC000-0xFFFF` | WIN3 — VRAM or extra data |

Stack grows downward from `0xBFFF`.

## Project Structure

```
sdcc-sprinter-sdk/
├── Makefile            # Main build system
├── README.md
├── include/
│   ├── sprinter.h      # Main header (includes all)
│   └── sprinter/
│       ├── types.h     # Integer types (u8, u16, etc.)
│       ├── ports.h     # Hardware I/O port definitions
│       ├── dss.h       # DSS OS API
│       ├── bios.h      # BIOS API
│       ├── video.h     # Video mode control
│       └── mouse.h     # Mouse driver
├── lib/
│   ├── crt0.s          # C runtime startup (sdasz80)
│   └── src/
│       ├── dss.c       # DSS system call wrappers
│       ├── bios.c      # BIOS call wrappers
│       ├── video.c     # Video functions
│       └── mouse.c     # Mouse functions
├── tools/
│   ├── ihx2exe.py      # Intel HEX → Sprinter EXE converter
│   ├── install-sdcc.sh # SDCC installer script
│   ├── build-sjasmplus.sh
│   └── bin/            # Built tools (sjasmplus)
├── examples/
│   ├── common.mk       # Shared build rules
│   ├── 01_hello/       # Hello World
│   ├── 02_files/       # File I/O
│   ├── 03_graphics/    # Graphics mode
│   ├── 04_mouse/       # Mouse input
│   └── 05_keyinput/    # Keyboard + system info
└── build/              # Compiled .rel files
```

## Based On

- **SOLID C** library (2004) — original C library for Sprinter with DSS/BIOS wrappers
- **zx-sprinter-sdk** — game development SDK reference
- **Estex DSS 1.60** documentation
- **SDCC** — Small Device C Compiler (sdcc.sourceforge.net)
- **sjasmplus** — Z80 cross-assembler (github.com/z00m128/sjasmplus)

## License

SDK source code and examples are provided for free use in ZX Sprinter development.
