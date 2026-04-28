# Memory Map and Linking

## Z80 Memory Map

The Z80 has a 64 KB address space divided into four 16 KB windows. DSS allocates 3 pages (WIN1, WIN2, WIN3) for each program.

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

### Compact Layout (CODE_LOC=0x8100)

```
0x4000 ├─────────────────────┤
       │  WIN1               │  Available for data
0x8000 ├─────────────────────┤
       │  WIN2               │  0x8100: Entry, code + data + stack
       │                     │  ~16 KB available
0xC000 ├─────────────────────┤
```

### Page2 Layout (CODE_LOC=0x4200, CRT0_PAGE2=1)

```
0x4000 ├─────────────────────┤  bootstrap stack (0x4000-0x40FF)
       │  WIN1               │  PSP (0x4100-0x41FF)
       │                     │  0x4200: Entry, code + initialised data
0x8000 ├─────────────────────┤  Runtime page allocated by CRT0
       │  WIN2               │  BSS / heap / working stack at 0xBFFF
0xC000 ├─────────────────────┤  WIN3 (DSS-allocated): free for the program
       │  WIN3               │
0xFFFF └─────────────────────┘
```

In this layout the EXE only contains the WIN1 image; CRT0 then asks DSS
for an extra page, maps it into WIN2 with `DSS.SETWIN2` and moves the
working stack to `0xBFFF`. This frees WIN1 below the entry point for a
larger PSP / bootstrap stack and keeps WIN2 distinct from the EXE image
so it can be remapped later (for example, swapping in another data page
while the working stack lives temporarily in the bootstrap area at
`0x40FF`).

To make the runtime exit cleanly, `crt0_page2` records the bootstrap SP
in the variable `_dss_exit_safe_sp` (defined in `lib/src/dss/dss_exit.s`).
`dss_exit()` consults this variable: if it is non-zero, `dss_exit` switches
SP back to the bootstrap stack before invoking DSS.LEAVE, so DSS can free
the runtime page without pulling the active stack out from under itself.
Programs that link the default `crt0` leave `_dss_exit_safe_sp = 0` and
`dss_exit` keeps the current SP, behaving exactly as before.

#### When to choose the page2 layout

The default and compact layouts always store code, initialised data and
BSS in a single linear region in WIN1+WIN2. Two situations make the
page2 layout the right choice instead:

1. **You want a small EXE on disk.** The on-disk EXE only covers the
   range from `CODE_LOC` to the highest initialised byte. With
   `CODE_LOC=0x4200` and a small program you can keep all code +
   initialised data inside WIN1 (≤ ~15.5 KB), so the EXE stays under
   16 KB even when the program needs full 64 KB worth of working memory
   at runtime. The default layout, in contrast, often pushes initialised
   data into WIN2, growing the EXE.

2. **You need a contiguous 32 KB region at runtime.** With code confined
   to WIN1 and the runtime stack at `0xBFFF`, addresses
   `0x8000–0xFFFF` (WIN2 + WIN3) are owned entirely by the program — no
   code, no initialised data — and the only thing you have to keep clear
   is the descending stack. That gives you up to **32 KB of contiguous
   workspace** for framebuffers, large arrays, decompressed assets, etc.,
   with WIN2 still free to be remapped to a different page when you need
   sliding-window access to more memory. The default layout cannot offer
   this: if any code or initialised data spills into WIN2, it sits in
   the middle of that workspace.

If neither of the above applies, prefer the default layout — it has the
simplest startup path and keeps DSS in charge of all three pages.

### Configuring Memory Layout

Override in your Makefile before `include`:

```makefile
CODE_LOC = 0x8100    # compact: code in WIN2 only
STACK    = 0xBFFF
```

For the page2 layout, set `CRT0_PAGE2=1` and `common.mk` will pick the
matching `CODE_LOC`, `STACK` and CRT0 object automatically:

```makefile
CRT0_PAGE2 = 1       # page2 layout: CRT0 allocates WIN2 page at startup
```

| Layout | CODE_LOC | STACK | CRT0 | Code+Data |
|--------|----------|-------|------|-----------|
| Default (SOLID C) | `0x4100` | `0xBFFF` | `crt0` | ~32 KB |
| Compact | `0x8100` | `0xBFFF` | `crt0` | ~16 KB |
| Extended | `0x4100` | `0xFFFE` | `crt0` | ~32 KB + WIN3 stack |
| Page2 (`CRT0_PAGE2=1`) | `0x4200` | `0x40FF`* | `crt0_page2` | ~15.5 KB code in WIN1; WIN2+WIN3 free for a 32 KB workspace |

\* Bootstrap stack only — `crt0_page2` switches the working stack to `0xBFFF` after allocating the WIN2 page.

#### Configuring the page2 layout

The minimum project Makefile is exactly one extra line — `CRT0_PAGE2=1`
must be set **before** including `common.mk`, since the include picks
`CODE_LOC`, `STACK` and the CRT0 object based on its value:

```makefile
APP        = small
SRCS       = main.c
SDK_DIR    = ../../
CRT0_PAGE2 = 1
include $(SDK_DIR)examples/common.mk
```

The build then produces a `small.exe` whose on-disk size matches just
your code + initialised data. After loading, the program owns
`0x8000-0xFFFF` as 32 KB of contiguous RAM (with the working stack at
the top of the range):

```c
#include <sprinter.h>

#define WORKSPACE      ((unsigned char *)0x8000)
#define WORKSPACE_LEN  (0x4000 - 0x0100)   /* leave room for the stack */

void main(void) {
    /* Use 0x8000..0xFEFF as one contiguous 16128-byte buffer.
       WIN2 was allocated by crt0_page2; WIN3 was allocated by DSS.    */
    for (unsigned i = 0; i < WORKSPACE_LEN; i++)
        WORKSPACE[i] = (unsigned char)i;
}
```

If you need to remap WIN2 to a different page mid-run (for example to
slide a window across a larger dataset), move the stack out of WIN2
first — either back to the bootstrap stack at `0x40FF` or to a region
inside WIN1 that you reserved as a static array — call
`dss_setwin(2, page)`, do the work, then put SP back. The descending
stack at `0xBFFF` is the only fixed cost in this layout; everything else
in `0x8000-0xFFFF` is yours to use.

You can also override `CRT0_PAGE2`'s defaults if needed:

```makefile
CRT0_PAGE2 = 1
CODE_LOC   = 0x4300  # leave more room below the entry for static data
STACK      = 0x42FF  # custom bootstrap stack, still inside WIN1
include $(SDK_DIR)examples/common.mk
```

The Sprinter has **4 MB RAM** (256 × 16 KB pages). For programs needing more than 32 KB, use `dss_getmem()` and `dss_setwin()` to map additional pages. SDCC also supports banked code via `--codeseg`.

## CRT0: C Runtime Startup

The CRT0 (`lib/crt0.s`) is the first code that runs when DSS loads your program. It performs three steps:

1. **Save command line** -- DSS passes the command line pointer in IX. CRT0 stores it in `__cmdline`.
2. **Initialize globals** -- copies initial values from the INITIALIZER area to INITIALIZED (for initialized global variables), then clears BSS (zero-initialized globals).
3. **Call main()** -- after initialization, CRT0 calls `_main`. When `main()` returns, CRT0 calls `DSS.Exit` with the return code.

The CRT0 must always be the first `.rel` file passed to the linker, so that `_entry` is placed at the start of the code segment (the configured `CODE_LOC`).

### Alternative CRT0 (`lib/crt0_page2.s`)

Built into `build/crt0_page2.rel` and selected by `CRT0_PAGE2=1` in
`common.mk`. Compared to the default CRT0 it adds two steps before
`gsinit`:

1. Records the bootstrap SP into the global `_dss_exit_safe_sp` so
   `dss_exit` can later restore it.
2. Calls `DSS.GETMEM` to obtain a 16 KB process page and `DSS.SETWIN2`
   to map it into WIN2, then switches SP to `0xBFFF` before running
   `gsinit` and `_main`.

If `DSS.GETMEM` fails the runtime calls `DSS.EXIT` with code `1`. The
EXE is built with `CODE_LOC=0x4200` and a `STACK` of `0x40FF`; the
working stack at `0xBFFF` is set up by the runtime, not by the EXE
header.

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
    u8 page;

    /* Allocate a memory page */
    page = dss_getmem();
    if (page == 0xFF) {
        dss_puts("Out of memory!\r\n");
        return;
    }

    /* Map it into WIN3 (0xC000-0xFFFF) */
    dss_setwin(3, page);

    /* Now 0xC000-0xFFFF points to the new page */
    /* Write data directly to addresses in that range */
    *((u8 *)0xC000) = 42;

    /* Free the page when done */
    dss_freemem(page);
}
```

**Warning:** Do not map pages over WIN2 (code) or WIN1 (data) while your program is running, as this will crash the system. Use WIN3 only when your stack/data layout leaves it free.
