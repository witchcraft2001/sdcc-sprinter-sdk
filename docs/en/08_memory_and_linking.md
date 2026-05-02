# Memory Map and Linking

## Z80 Memory Map

The Z80 has a 64 KB address space divided into four 16 KB windows. DSS allocates pages for the loaded EXE image; a stack address in the EXE header does not by itself reserve that window for the process.

### Default Layout (CODE_LOC=0x4100, like SOLID C)

```
0x0000 ┌─────────────────────┐
       │  WIN0               │  System / available for banking
       │  (0x0000 - 0x3FFF)  │
0x4000 ├─────────────────────┤
       │  WIN1               │  0x4080-0x40FF: Command line args
       │  (0x4000 - 0x7FFF)  │  0x4100: Program entry (_entry)
       │                     │  Code + data grows upward
0x8000 ├─────────────────────┤
       │  WIN2               │  Code + data (continued)
       │  (0x8000 - 0xBFFF)  │  Stack grows downward from 0xBFFF
0xC000 ├─────────────────────┤
       │  WIN3               │  VRAM mapping or extra data
       │  (0xC000 - 0xFFFF)  │
0xFFFF └─────────────────────┘
```

**Total code+data:** 0x4100–0xBFFF ≈ **32 KB** (WIN1 + WIN2).

This layout is safe only when the loaded image actually reaches WIN2 or the program otherwise allocates WIN2 before using the stack there. A tiny `CODE_LOC=0x4100` program can fit entirely in WIN1 while still declaring `STACK=0xBFFF`; in that case DSS may not preserve WIN2 across `EXEC`, so parent-stack based APIs can return incorrectly.

### Compact Layout (CODE_LOC=0x8100)

```
0x4000 ├─────────────────────┤
       │  WIN1               │  Available for data
0x8000 ├─────────────────────┤
       │  WIN2               │  0x8100: Entry, code + data + stack
       │                     │  ~16 KB available
0xC000 ├─────────────────────┤
```

### Configuring Memory Layout

Override in your Makefile before `include`:

```makefile
CODE_LOC = 0x8100    # compact: code in WIN2 only
STACK    = 0xBFFF
```

| Layout | CODE_LOC | STACK | Code+Data |
|--------|----------|-------|-----------|
| Default (SOLID C) | `0x4100` | `0xBFFF` | ~32 KB |
| Compact | `0x8100` | `0xBFFF` | ~16 KB |
| Extended | `0x4100` | `0xFFFE` | ~32 KB + WIN3 stack |
| Page2 (`CRT0_PAGE2=1`) | `0x4200` | `0x40FF` bootstrap, then `0xBFFF` | code in WIN1; CRT0 allocates WIN2 |

Use `CRT0_PAGE2=1` when a small program fits in WIN1 but needs a real process-owned stack in WIN2. The alternate CRT0 starts on the WIN1 bootstrap stack, calls `DSS.GETMEM` and `DSS.SETWIN2`, then switches `SP` to `0xBFFF`. This is required for small programs that call `dss_exec()` or `dss_wait()`, because DSS task switching restores only pages that belong to the process.

The Sprinter has **4 MB RAM** (256 × 16 KB pages). For programs needing more than 32 KB, use `dss_getmem_pages()` and `dss_setwin_page()` to map pages from an allocated DSS block. SDCC also supports banked code via `--codeseg`.

Example:

```makefile
APP        = small
SRCS       = main.c
CRT0_PAGE2 = 1
include ../common.mk
```

## CRT0: C Runtime Startup

The CRT0 (`lib/crt0.s`) is the first code that runs when DSS loads your program. It performs three steps:

1. **Save command line** -- DSS passes the command line pointer in IX. CRT0 stores it in `__cmdline`.
2. **Initialize globals** -- copies initial values from the INITIALIZER area to INITIALIZED (for initialized global variables), then clears BSS (zero-initialized globals).
3. **Call main()** -- after initialization, CRT0 calls `_main`. When `main()` returns, CRT0 calls `DSS.Exit` with the return code.

The CRT0 must always be the first `.rel` file passed to the linker, so that `_entry` is placed at address 0x8100.

`lib/crt0_page2.s` is the alternate startup used by `CRT0_PAGE2=1`. It must be linked with `CODE_LOC=0x4200` and `STACK=0x40FF`; after startup it allocates WIN2 and moves the working stack to `0xBFFF`.

## Selective Linking

### How It Works

Each library function is compiled into a separate `.rel` object file (e.g., `puts.rel`, `strlen.rel`, `dss_open.rel`). All modules are packed into `sprinter.lib` using `sdar` (SDCC archive tool).

When the linker resolves your program's symbols, it pulls only the referenced modules from the archive. Unused functions contribute zero bytes to the final binary.

### Dependency Chain Example

If your program calls `printf("hello %d", n)`, the linker pulls in:

```
printf.rel -> _vprintfmt (printf engine)
           -> fputc.rel -> dss_putchar.rel
           -> _stdio_init.rel (FILE table)
```

Total: approximately 2.8 KB of code.

If your program only calls `puts("hello")`, the chain is shorter:

```
puts.rel -> dss_putchar.rel
```

Total: approximately 170 bytes.

### Code Size Comparison

| Program | Functions Used | Approximate Size |
|---------|---------------|-----------------|
| `dss_puts()` only | dss_puts | ~170 B |
| `puts()` | puts, dss_putchar | ~200 B |
| `printf()` | printf, _vprintfmt, fputc, dss_putchar, _stdio_init | ~2.8 KB |
| `printf()` + `fopen()/fwrite()` | above + fopen, fwrite, dss_open, dss_write | ~3.6 KB |
| `printf()` + `string.h` + `ctype.h` | many modules | ~3.6 KB |
| Full sort demo (example 08) | printf, cprintf, rand, srand + sort logic | ~4.3 KB |

### Tips for Reducing Code Size

1. **Use `dss_puts()` instead of `printf()`** when you do not need formatting. Saves ~2.6 KB.

2. **Use `cputs()` instead of `cprintf()`** for plain strings.

3. **Avoid `sprintf()`** unless needed -- it pulls in the full printf engine.

4. **Prefer `dss_*` functions** over `stdio.h` equivalents when binary size matters. For example, `dss_open` / `dss_write` / `dss_close` is smaller than `fopen` / `fwrite` / `fclose`.

5. **Minimize global data.** String literals in code count toward the data segment. Use shorter messages or combine related strings.

6. **Use SDCC optimization flags.** The SDK defaults are already good: `--max-allocs-per-node 5000 --opt-code-speed`.

## DSS EXE Format

The `ihx2exe.py` tool converts SDCC's Intel HEX output into the Sprinter EXE format:

```
Offset  Size  Description
0x0000  512   EXE header
0x0200  ...   Program binary (loaded at 0x8100)
```

The header contains:

- EXE version (`exe_ver = 1`, current format)
- Load address (0x8100)
- Entry point (0x8100)
- Program size
- DSS signature

DSS reads the header, loads the binary into memory at the specified address, sets the stack pointer, and jumps to the entry point.

## Using Additional Memory Pages

For programs that need more than 32 KB of data (16 KB code + 16 KB data), you can allocate and map additional memory pages:

```c
#include <sprinter.h>

void main(void) {
    u8 block;

    /* Allocate a 2-page block */
    block = dss_getmem_pages(2);
    if (block == 0xFF) {
        dss_puts("Out of memory!\r\n");
        return;
    }

    /* Map page 1 of the block into WIN3 (0xC000-0xFFFF) */
    dss_setwin_page(3, block, 1);

    /* Now 0xC000-0xFFFF points to that block page */
    /* Write data directly to addresses in that range */
    *((u8 *)0xC000) = 42;

    /* Free the whole block when done */
    dss_freemem(block);
}
```

**Warning:** Do not map pages over WIN2 (code) or WIN1 (data) while your program is running, as this will crash the system. WIN0 is reserved by DSS; use WIN3 for extra data pages.
