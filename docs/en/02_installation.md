# Installation

## Prerequisites

You need three tools to build the SDK and compile programs:

| Tool | Version | Purpose |
|------|---------|---------|
| **SDCC** | 2.9.0 | C compiler, assembler, linker, library archiver |
| **Python** | 3.x | `ihx2exe.py` converter (Intel HEX to Sprinter EXE) |
| **GNU Make** | 3.8+ | Build system |

## macOS

Install via Homebrew:

```bash
brew install python3
```

GNU Make is included with Xcode Command Line Tools:

```bash
xcode-select --install
```

## Linux

**Ubuntu / Debian:**

```bash
sudo apt install python3 make
```

**Fedora / RHEL:**

```bash
sudo dnf install python3 make
```

**Arch Linux:**

```bash
sudo pacman -S python3 make
```

## Windows

### Method 1: MSYS2 (recommended)

1. Download and install [MSYS2](https://www.msys2.org/).
2. Open the **MSYS2 MinGW64** terminal.
3. Install packages:

```bash
pacman -S mingw-w64-x86_64-python3 make
```

4. Use the MSYS2 MinGW64 terminal for all SDK commands.

### Method 2: Manual Installation

1. Download and install [Python 3.x](https://www.python.org/downloads/). During installation, check **"Add Python to PATH"**.
2. Install the SDCC 2.9.0 toolchain.
   The recommended setup is to point the SDK at the original SDCC 2.9.0 `bin` directory.
   That directory should contain `sdcc` or `sdcc-2.9.0`, `sdcpp-2.9.0`, and either the upstream tool names `as-z80-2.9.0`, `sdcclib-2.9.0`, `link-z80-2.9.0` or repackaged equivalents `sdasz80`, `sdar`, `sdldz80`.
3. Install [GNU Make](https://gnuwin32.sourceforge.net/packages/make.htm), or use Make from Git Bash / MSYS2.

## Verifying Installation

Run these commands to confirm everything is installed:

```bash
/absolute/path/to/sdcc-2.9.0/bin/sdcc --version
python3 --version
```

Expected output for the compiler: `SDCC : mcs51/z80/... 2.9.0 ...`

If the compiler binary is named `sdcc-2.9.0`, use that full path instead.

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

If several SDCC versions are installed on the same machine, pin this SDK to the exact 2.9.0 toolchain directory:

```bash
make SDCC290_BIN_DIR=/absolute/path/to/sdcc-2.9.0/bin
make examples SDCC290_BIN_DIR=/absolute/path/to/sdcc-2.9.0/bin
```

You can also create `config.local.mk` from `config.local.mk.example` and store the path there:

```makefile
SDCC290_BIN_DIR := /absolute/path/to/sdcc-2.9.0/bin
```

If you prefer not to use `SDCC290_BIN_DIR`, define the tool paths explicitly:

```makefile
SDCC    := /absolute/path/to/sdcc-2.9.0/bin/sdcc
SDCPP   := /absolute/path/to/sdcc-2.9.0/bin/sdcpp-2.9.0
SDASZ80 := /absolute/path/to/sdcc-2.9.0/bin/as-z80-2.9.0
SDAR    := /absolute/path/to/sdcc-2.9.0/bin/sdcclib-2.9.0
SDLDZ80 := /absolute/path/to/sdcc-2.9.0/bin/link-z80-2.9.0
```

## Building the Examples

```bash
make examples
```

This builds all 19 example programs. Each example produces an `.exe` file in its directory (e.g., `examples/01_hello/hello.exe`).

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
├── examples/                # 19 example programs
│   ├── common.mk            # Shared Makefile rules
│   ├── 01_hello/ .. 19_console/
├── scripts/
│   ├── copy_exe.sh          # Copy EXE files to target directory
│   └── make_floppy.sh       # Create FAT12 floppy image
└── docs/                    # Documentation
```
