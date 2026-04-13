# Installation

## Prerequisites

You need three tools to build the SDK and compile programs:

| Tool | Version | Purpose |
|------|---------|---------|
| **SDCC** | 4.x | C compiler, assembler, linker, library archiver |
| **Python** | 3.x | `ihx2exe.py` converter (Intel HEX to Sprinter EXE) |
| **GNU Make** | 3.8+ | Build system |

## macOS

Install via Homebrew:

```bash
brew install sdcc python3
```

GNU Make is included with Xcode Command Line Tools:

```bash
xcode-select --install
```

## Linux

**Ubuntu / Debian:**

```bash
sudo apt install sdcc python3 make
```

**Fedora / RHEL:**

```bash
sudo dnf install sdcc python3 make
```

**Arch Linux:**

```bash
sudo pacman -S sdcc python3 make
```

## Windows

### Method 1: MSYS2 (recommended)

1. Download and install [MSYS2](https://www.msys2.org/).
2. Open the **MSYS2 MinGW64** terminal.
3. Install packages:

```bash
pacman -S mingw-w64-x86_64-sdcc mingw-w64-x86_64-python3 make
```

4. Use the MSYS2 MinGW64 terminal for all SDK commands.

### Method 2: Manual Installation

1. Download and install [Python 3.x](https://www.python.org/downloads/). During installation, check **"Add Python to PATH"**.
2. Download and install [SDCC](https://sdcc.sourceforge.net/) using the Windows installer. Make sure it is added to PATH.
3. Install [GNU Make](https://gnuwin32.sourceforge.net/packages/make.htm), or use Make from Git Bash / MSYS2.

## Verifying Installation

Run these commands to confirm everything is installed:

```bash
sdcc --version
```

Expected output: `SDCC : mcs51/z80/... 4.x.x ...`

```bash
python3 --version
```

Expected output: `Python 3.x.x`

```bash
make --version
```

Expected output: `GNU Make 3.x` or `GNU Make 4.x`

On Windows, you may need to use `python` instead of `python3`.

## Building the SDK

Clone or download the SDK, then build the library:

```bash
cd sdcc-sprinter-sdk
make
```

This compiles all library modules and creates:

- `build/crt0.rel` -- C runtime startup object
- `build/sprinter.lib` -- library archive (~103 modules, selective linking)

## Building the Examples

```bash
make examples
```

This builds all 13 example programs. Each example produces an `.exe` file in its directory (e.g., `examples/01_hello/hello.exe`).

## Building sjasmplus (optional)

Only needed if you plan to write mixed C + assembly projects:

```bash
make tools
```

This downloads, compiles, and installs `sjasmplus` (Z80 cross-assembler) into the `tools/bin/` directory.

## Directory Structure After Build

```
sdcc-sprinter-sdk/
├── Makefile                 # Top-level build
├── README.md
├── build/
│   ├── crt0.rel             # Compiled CRT0 startup
│   ├── sprinter.lib         # Library archive (~103 modules)
│   └── *.rel                # Individual compiled modules
├── include/
│   ├── stdio.h              # Standard C I/O
│   ├── stdlib.h             # Standard utilities
│   ├── string.h             # String operations
│   ├── ctype.h              # Character classification
│   ├── conio.h              # Console I/O
│   ├── stddef.h             # size_t, NULL, offsetof
│   ├── stdbool.h            # bool, true, false
│   ├── sprinter.h           # Umbrella header (includes all sprinter/* headers)
│   └── sprinter/
│       ├── types.h          # Integer types: u8, u16, i8, i16, u32, i32
│       ├── ports.h          # Hardware I/O port definitions
│       ├── dss.h            # DSS OS API
│       ├── bios.h           # BIOS API
│       ├── video.h          # Video mode control
│       └── mouse.h          # Mouse driver
├── lib/
│   ├── crt0.s               # C runtime startup (assembly source)
│   └── src/                 # Library source (one function per file)
│       ├── dss/             # DSS wrappers (30 modules)
│       ├── bios/            # BIOS wrappers (6 modules)
│       ├── video/           # Video functions (7 modules)
│       ├── mouse/           # Mouse functions (7 modules)
│       ├── stdio/           # Standard I/O (20 modules)
│       ├── stdlib/          # Standard library (4 modules)
│       ├── string/          # String functions (12 modules)
│       ├── ctype/           # Character functions (12 modules)
│       └── conio/           # Console I/O (8 modules)
├── tools/
│   ├── ihx2exe.py           # Intel HEX → Sprinter EXE converter
│   ├── install-sdcc.sh      # SDCC installer helper
│   └── build-sjasmplus.sh   # sjasmplus build script
├── examples/                # 13 example programs
│   ├── common.mk            # Shared Makefile rules
│   ├── 01_hello/ .. 13_random/
├── scripts/
│   ├── copy_exe.sh          # Copy EXE files to target directory
│   └── make_floppy.sh       # Create FAT12 floppy image
└── docs/                    # Documentation
```
