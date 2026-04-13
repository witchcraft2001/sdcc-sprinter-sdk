# Quick Start

## Minimal Program Using stdio.h

The simplest way to write a Sprinter program is to use the standard C library:

```c
#include <stdio.h>

void main(void) {
    printf("Hello, Sprinter!\n");
    getchar();
}
```

This uses `printf()` for output and `getchar()` to wait for a key press before exiting. The `printf` engine adds about 2.8 KB to your binary.

## Minimal Program Using Sprinter API

For smaller binaries, use the Sprinter-specific API directly:

```c
#include <sprinter.h>

void main(void) {
    dss_puts("Hello, Sprinter!\r\n");
    dss_waitkey();
}
```

This program is only ~170 bytes because `dss_puts()` is a thin wrapper around the DSS system call. Note: DSS expects `\r\n` for newlines (unlike `printf` which translates `\n` automatically).

## Creating a Makefile

Create a `Makefile` in your project directory with three lines:

```makefile
APP      = hello
SRCS     = main.c
SDK_DIR  = /path/to/sdcc-sprinter-sdk/
include $(SDK_DIR)examples/common.mk
```

- **APP** -- output filename (without `.exe` extension)
- **SRCS** -- list of C source files
- **SDK_DIR** -- absolute or relative path to the SDK root (must end with `/`)

For multiple source files:

```makefile
APP      = myapp
SRCS     = main.c utils.c gfx.c
SDK_DIR  = /path/to/sdcc-sprinter-sdk/
include $(SDK_DIR)examples/common.mk
```

## Building

```bash
make
```

This produces `hello.exe` in your project directory.

## Running on Emulator or Hardware

Copy the `.exe` file to the Sprinter's filesystem (real hardware or emulator) and run from the DSS command line:

```
A:\>HELLO.EXE
```

## Build Process Explained

The build pipeline has four stages:

```
main.c  ──sdcc──>  main.rel  ──sdldz80──>  hello.ihx  ──ihx2exe.py──>  hello.exe
  (C source)      (relocatable     (Intel HEX,        (Sprinter EXE,
                   object)          absolute code)      512-byte header
                                                        + code)
```

### Stage 1: Compile (.c to .rel)

```bash
sdcc -mz80 -I<sdk>/include -c -o _build/main.rel main.c
```

SDCC compiles each `.c` file to a relocatable object file (`.rel`). The `-mz80` flag selects the Z80 target. The `-c` flag means "compile only, do not link."

### Stage 2: Link (.rel to .ihx)

```bash
sdcc -mz80 --no-std-crt0 --code-loc 0x8100 --data-loc 0x4000 \
    crt0.rel main.rel -lsprinter.lib -o _build/hello.ihx
```

The linker combines `crt0.rel` (startup code), your object files, and any needed modules from `sprinter.lib` into an Intel HEX file. Key flags:

- `--no-std-crt0` -- use the SDK's custom CRT0 instead of SDCC's built-in
- `--code-loc 0x8100` -- place code at address 0x8100 (DSS loads programs here)
- `--data-loc 0x4000` -- place data segment at address 0x4000 (WIN1)

### Stage 3: Convert (.ihx to .exe)

```bash
python3 ihx2exe.py _build/hello.ihx hello.exe
```

The `ihx2exe.py` script converts the Intel HEX file to the Sprinter DSS EXE format: a 512-byte header followed by the raw binary code. DSS reads this header to know the load address and entry point.

### What common.mk Does

The shared Makefile (`examples/common.mk`) automates all three stages. When you type `make`, it:

1. Creates the `_build/` directory
2. Compiles each source file in `SRCS` to `_build/*.rel`
3. Links with CRT0 and `sprinter.lib` to produce `_build/<APP>.ihx`
4. Converts to `<APP>.exe`

Build artifacts go into `_build/` to keep the project directory clean.
