# Introduction

## What is SDCC Sprinter SDK

SDCC Sprinter SDK is a cross-platform C development toolkit for the **ZX Sprinter** computer. It provides a standard C library, Sprinter hardware API bindings, build tools, and example programs -- everything you need to write C programs for the Sprinter using the free SDCC compiler.

The SDK was designed with two goals in mind:

1. **Ease of porting** -- standard C headers (`stdio.h`, `string.h`, `conio.h`, etc.) let you compile existing C programs with minimal changes.
2. **Full hardware access** -- Sprinter-specific headers give direct access to DSS OS calls, BIOS, video modes, mouse driver, and hardware ports.

## Target Platform

**ZX Sprinter** (also known as Peters MC Sprinter 2000) is a Z80-based computer with the following characteristics:

- **CPU:** Z80 at 7 MHz (turbo mode: 21 MHz)
- **RAM:** 4 MB (paged in 16 KB windows)
- **Video:** 320x256 at 256 colors, 640x256 at 16 colors, ZX Spectrum compatible text modes
- **Sound:** AY-3-8910 + Covox DAC
- **Storage:** IDE HDD, FDD, CF card
- **OS:** Estex DSS (Disk SubSystem) -- a CP/M-like operating system with FAT filesystem support

DSS provides system calls for file I/O, console output, keyboard input, memory management, process execution, and more. The SDK wraps these calls in C functions.

## SDCC Compiler

**SDCC** (Small Device C Compiler) is a free, open-source, retargetable C compiler for 8-bit processors including the Z80. It runs on macOS, Linux, and Windows.

Key features relevant to Sprinter development:

- **C89/C99 support** (partial C11/C23)
- **Z80 code generation** with register-based calling convention (`sdcccall(1)`)
- **Optimizing compiler** with configurable optimization levels
- **Standard linker** (`sdldz80`) with library archive support (`sdar`)
- **Intel HEX output** (converted to Sprinter EXE by `ihx2exe.py`)

Website: [https://sdcc.sourceforge.net/](https://sdcc.sourceforge.net/)

## Comparison with SOLID C

SOLID C was the original C compiler for ZX Sprinter, bundled with a DSS/BIOS library. Here is how the SDCC SDK compares:

| Feature | SOLID C | SDCC Sprinter SDK |
|---------|---------|-------------------|
| Compiler | Proprietary, Sprinter-only | Free, cross-platform (macOS/Linux/Windows) |
| Language | K&R C + some ANSI | ANSI C89/C99 |
| Calling convention | Register-based (custom) | Register-based (`sdcccall(1)`) |
| Standard library | Partial (`stdio.h`, `string.h`, etc.) | Comparable (`stdio.h`, `stdlib.h`, `string.h`, `ctype.h`, `conio.h`) |
| Recursion | Requires `#pragma nonrec` or `recursive` keyword | Automatic (SDCC handles stack frames) |
| Variadic functions | `.` notation | Standard `<stdarg.h>` with `...` |
| Dynamic memory | `malloc` / `free` available | Not yet implemented |
| Optimization | Basic | Configurable (`--max-allocs-per-node`, `--opt-code-speed`) |
| Linking | All-or-nothing | Selective (only used functions are linked) |
| Development | Discontinued (2004) | Active |

See [Porting from SOLID C](07_porting_from_solidc.md) for a detailed migration guide.

## SDK Features

- **Standard C library** -- `stdio.h`, `stdlib.h`, `string.h`, `ctype.h`, `conio.h`, `stddef.h`, `stdbool.h`
- **Sprinter hardware API** -- DSS OS calls, BIOS functions, video modes, mouse driver, port I/O
- **Selective linking** -- each function compiled as a separate module (~103 modules in `sprinter.lib`); the linker includes only what your program actually uses
- **15 example programs** -- from "Hello World" to EXEC/error handling demo
- **Build tools** -- `ihx2exe.py` (Intel HEX to Sprinter EXE converter), `common.mk` (shared Makefile rules)
- **Cross-platform** -- works on macOS, Linux, and Windows (MSYS2/MinGW)
- **sjasmplus support** -- optional Z80 assembler for mixed C+ASM projects
