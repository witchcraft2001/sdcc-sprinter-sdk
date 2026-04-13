# Memory Map and Linking

## Z80 Memory Map

The Z80 has a 64 KB address space divided into four 16 KB windows. DSS maps RAM pages into these windows:

```
0x0000 ┌─────────────────────┐
       │  WIN0               │  Available for data
       │  (0x0000 - 0x3FFF)  │  DSS kernel page (restored on exit)
0x4000 ├─────────────────────┤
       │  WIN1               │  Data segment (--data-loc 0x4000)
       │  (0x4000 - 0x7FFF)  │  Global/static variables, string literals
0x8000 ├─────────────────────┤
       │  WIN2               │  Code + Stack
       │  (0x8000 - 0xBFFF)  │  0x8000-0x80FF: EXE header (disk only)
       │                     │  0x8100: Program entry point (_entry)
       │                     │  Code grows upward from 0x8100
       │                     │  Stack grows downward from 0xBFFF
0xC000 ├─────────────────────┤
       │  WIN3               │  VRAM mapping or extra data
       │  (0xC000 - 0xFFFF)  │
0xFFFF └─────────────────────┘
```

### Memory Window Usage

| Window | Address Range | Linker Flag | Usage |
|--------|--------------|-------------|-------|
| WIN0 | 0x0000-0x3FFF | -- | Available; DSS kernel on entry |
| WIN1 | 0x4000-0x7FFF | `--data-loc 0x4000` | Data segment (globals, statics, strings) |
| WIN2 | 0x8000-0xBFFF | `--code-loc 0x8100` | Code + stack |
| WIN3 | 0xC000-0xFFFF | -- | VRAM or user-mapped pages |

### Code Size Budget

In the default SDK configuration, code and stack share WIN2 (0x8100–0xBFFF, ~15.7 KB). The stack pointer starts at 0xBFFF and grows downward.

Data occupies WIN1 (16 KB, 0x4000–0x7FFF). Global arrays, static variables, and string constants all go here.

**Default total:** ~16 KB for code + 16 KB for data in two windows.

This is a limitation of the **default memory layout**, not the hardware. The Sprinter has **4 MB RAM** (256 × 16 KB pages). For larger programs, use `dss_getmem()` and `dss_setwin()` to map additional pages into WIN0 or WIN3. SDCC also supports banked code via `--codeseg` for programs exceeding a single window.

## CRT0: C Runtime Startup

The CRT0 (`lib/crt0.s`) is the first code that runs when DSS loads your program. It performs three steps:

1. **Save command line** -- DSS passes the command line pointer in IX. CRT0 stores it in `__cmdline`.
2. **Initialize globals** -- copies initial values from the INITIALIZER area to INITIALIZED (for initialized global variables), then clears BSS (zero-initialized globals).
3. **Call main()** -- after initialization, CRT0 calls `_main`. When `main()` returns, CRT0 calls `DSS.Exit` with the return code.

The CRT0 must always be the first `.rel` file passed to the linker, so that `_entry` is placed at address 0x8100.

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
    u8 page;

    /* Allocate a memory page */
    page = dss_getmem();
    if (page == 0xFF) {
        dss_puts("Out of memory!\r\n");
        return;
    }

    /* Map it into WIN0 (0x0000-0x3FFF) */
    dss_setwin(0, page);

    /* Now 0x0000-0x3FFF points to the new page */
    /* Write data directly to addresses in that range */
    *((u8 *)0x0000) = 42;

    /* Free the page when done */
    dss_freemem(page);
}
```

**Warning:** Do not map pages over WIN2 (code) or WIN1 (data) while your program is running, as this will crash the system. Use WIN0 or WIN3 for extra data pages.
