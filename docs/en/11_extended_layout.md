# 11. Extended WIN0..WIN2 Layout (~47 KB Programs)

The standard layout gives a program ~32 KB (code+data across WIN1+WIN2, see
[08_memory_and_linking](08_memory_and_linking.md)). The **extended WIN0..WIN2
layout** raises that to **~47 KB** by running the program from WIN0 as well and
routing every BIOS/DSS/interrupt access through trampolines.

You opt in with a one-line Makefile (`include ../win0.mk`); ordinary C —
including `printf` and stdio — builds unchanged.

## 11.1. The idea

Normally a program lives in WIN1+WIN2 (`0x4000-0xBFFF`) while WIN0
(`0x0000-0x3FFF`) holds the resident DSS core page. The extended layout puts the
program's own page into WIN0 too, and intercepts the `RST` vectors
(`#08` BIOS, `#10` DSS, `#18` disk, `#30` mouse, `#38` IM1) with trampolines that
swap the DSS core page back into WIN0 for the duration of each firmware call,
then restore the program page.

This is not speculative: it is exactly how the resident DSS works (its WIN0 page
carries the RST vector table, `Estex-DSS/DSS/DSS-MAIN.ASM`, `ORG 0`) and how the
**Tasm** editor runs (`Tasm_Main.asm` — own WIN0 page + RST
trampolines + a primary loader in `TasmEXE.asm`).

## 11.2. Hardware primitives

| Primitive | Value |
|---|---|
| Window selectors (16 KB page) | WIN0=`#82`, WIN1=`#A2`, WIN2=`#C2`, WIN3=`#E2` |
| Border | port `#FE`, bits 0-2 |
| RST dispatchers | BIOS=`#08`, DSS=`#10`, disk=`#18`, mouse=`#30`, IM1=`#38`; selector in `C` |

`IN A,(#82)` reads the current WIN0 page; at program start that is the DSS core
page.

## 11.3. What is in WIN0 at runtime

WIN0 holds the **DSS core RAM page** ("COREPAGE"), allocated dynamically from the
EMM when DSS deploys (its physical number is **not** a fixed constant). That page
carries the RST vector table (the `RST_0x08` stub, the `RST_0x10` dispatcher,
`RST_0x18/0x30`, the `RST_0x38` IM1 handler) and all DSS `CORE_BUFFERS` (below
`0x4000`). `RST #10` dispatches only while that page is in WIN0.

**Consequence:** capture `dss_page = IN A,(#82)` at start; for each firmware call,
map `dss_page` into WIN0, then restore the program page afterwards.

## 11.4. The single hard constraint, and pointer marshalling

The constraint is not "DSS repages everything" — it is that **our trampoline puts
the DSS page into WIN0 during a call**. Therefore:

> A pointer handed to firmware must **not** point into WIN0 (`0x0000-0x3FFF`).
> Pointers in WIN1/WIN2/WIN3 are fine.

This is handled by the win0 `dss_*` **override wrappers** (`lib/win0/win0_dss.c`),
which gate on `ptr < 0x4000` and bounce WIN0 pointers through a WIN2 staging
buffer before/after the call, then invoke the raw wrapper (`dss_*_raw`,
`lib/win0/dss_raw.s`). Important properties:

* These overrides are linked **only** in the win0 layout, so the normal layout
  pays **zero overhead** (it uses the library's raw `dss_*` directly).
* For a pointer already at `≥0x4000` (the common case) the override just does one
  compare + a tail call to the raw wrapper — no copy.
* The bounce (a staging copy) happens only for the rare WIN0-resident pointer:
  inline string literals (SDCC keeps these in `_CODE`) and the command-line
  string. Input strings are copied before the call; output buffers after; streams
  and large reads/writes are chunked; fixed structs use `sizeof`.

Functions that only fill a caller struct **in the wrapper after the RST** (passing
just values to DSS) are already win0-safe and are **not** wrapped:
`getdate/gettime/settime/meminfo/waitkey_ex/scankey/testkey`. Likewise `printf`
and other value-passing calls are transparent (they read their arguments
themselves, in program context).

## 11.5. Memory map (running program)

```
            WIN0 (P0, #82)              WIN1 (P1)        WIN2 (P2, #C2 - never repaged)
0x0000 ┌ RST vector table (JP stubs) ┐
0x0040 │ boot params {dss,p0,p1,p2}   │
0x0066 │ NMI (RETN)                   │
0x0080 │ command line (<=256)         │
0x0180 │ _CODE start ───────────────►│ _CODE ──────────►│ _CODE tail
       │                             │ 0x4000..0x7FFF   │ _DATA / _BSS / _HEAP / staging
       │                             │                  │ stack (down from ~0xBEFF)
0x3FFF └─────────────────────────────┘  0x7FFF          │ 0x8000: trampolines (_WINRT)
                                                         │ 0xBF00: stack top, 0xBFFF
```

* Payload link origins: `-b _CODE=0x0180 -b _WINRT=0x8000 -b _DATA=0x8400`,
  stack `0xBF00`.
* Code/rodata may span WIN0+WIN1 (~31 KB). WIN2 is the program's **private**
  window (data, stack, trampolines), never scratched by DSS.
* Budget: ~32 KB code (WIN0+WIN1) + ~15 KB data/stack (WIN2) ≈ **47 KB**.

## 11.6. Two-stage PRELOAD architecture

DSS `EXEC` cannot load a program into WIN0 (it maps from the window containing
`LD_ADDR`, and WIN0 is the system page during EXEC). So a small stage-1 loader
populates WIN0 after start.

**Stage 1 — loader** (a PRELOAD `.EXE` at `0x8100`). The `.EXE` is built by
`tools/win0_exe.py` as `[512-byte header][loader][size table][WIN0 blob][WIN1
blob][WIN2 blob]`. The header's `LOADER` field makes DSS load only the loader,
leave the file open, and jump; the open handle is in the PSP (`*(u8*)0x807D`).
The loader:

1. `dss_page = IN A,(#82)`.
2. `GETMEM` P0/P1/P2, read their physical numbers (`SETWIN`+`IN`).
3. Stream the blobs from the still-open `.EXE` file straight into P0/P1/P2 via
   `DSS.Read`.
4. Copy the command line from its PSP into `P0:0x0080`; write boot params at
   `P0:0x0040`.
5. Map P0 into WIN0, P1 into WIN1, and `JP 0x0180`.

**Stage 2 — `crt0_win0.s`** (entry `0x0180`): map WIN1=P1 and WIN2=P2, set
`SP=0xBF00`, run `gsinit`, copy the boot params into the WIN2 page table, install
the RST vectors into `P0:0x0008/0x0010/0x0038`, `EI`, call `main`, and on return
exit via the RST10 trampoline (`DSS.Exit`).

## 11.7. RST trampolines and IM1

The vector table lives at `P0:0x0000-0x003F` (3-byte `JP` stubs). The trampoline
bodies live in `_WINRT` at `0x8000` (WIN2), because a trampoline changes WIN0
"from under itself" — it must execute from a window that is never repaged.

DSS trampoline (BIOS and mouse are analogous). Two correctness rules:

* **Only WIN0 is switched** to the DSS core page (WIN2 is never touched).
* **The caller's interrupt state is preserved, not forced.** The trampoline
  samples IFF on entry (`LD A,I` → P/V) and re-enables interrupts only if the
  caller had them enabled — some BIOS calls require DI on entry, so it must never
  implicitly enable them. The decision is carried by self-modifying code (the
  trampoline is in WIN2 RAM): the in-place `EI`/`NOP` opcode bytes (`0xFB`/`0x00`)
  are patched at entry — no data cell, no branch.

```
rst10_tramp:                      ; in WIN2
        push af                    ; save caller AF (A may be a param)
        ld   a,i                  ; P/V = caller IFF2
        di                        ; protect the WIN0 swap
        ld   a,#0x00 : jp po,1$ : ld a,#0xFB   ; 0xFB=EI if enabled, 0x00=NOP if not
1$:     ld   (2$),a : ld (3$),a   ; SMC-patch the two in-place EI/NOP bytes
        ld   a,(dss_page) : out (#82),a   ; WIN0 = DSS core page -> 0x0010 = dispatcher
        pop  af                   ; restore caller AF
2$:     nop                       ; <- EI/NOP: caller's INT state for the call
        rst  #10                  ; real DSS call
        di
        push af                   ; save return AF (A + CF)
        ld   a,(prog_w1) : out (#A2),a    ; restore program windows
        ld   a,(prog_w0) : out (#82),a
        pop  af
3$:     nop                       ; <- EI/NOP: caller's INT state
        ret
```
(`LD A,I` carries the known Z80 erratum if an interrupt lands exactly during it,
which can at worst leave interrupts disabled for one call — rare and acceptable.
See `lib/win0/win0_rt.s` for the exact code.)

IM1 trampoline: while the program runs, an interrupt vectors to
`P0:0x0038 → rst38_tramp` (WIN2). It saves registers, maps WIN0=DSS core, pushes
a resume address and jumps to the DSS `0x0038` handler (which does keyscan/mouse/
cursor and ends in `EI/RETI` → returns to our resume stub), then restores WIN0=P0
and `RETI`. So DSS's per-frame work keeps running under our WIN0 page.

## 11.8. Building a program in the extended layout

The application Makefile is one line:

```make
APP  = myapp
SRCS = main.c
include ../win0.mk
```

This builds the payload (your code in WIN0+WIN1, the runtime trampolines in WIN2),
packs it with the stage-1 loader into a single `.EXE`, and links everything.

### Coding rules — ordinary C, no changes

* `printf`/`putchar` and anything passing **values** to DSS work as is.
* Direct and library-internal DSS pointers (`dss_puts`, `dss_open`, `fopen`,
  `fread`/`fwrite`, …) work as is: the win0 build links **overriding** `dss_*`
  wrappers that handle WIN0 pointers. The normal layout does not link them →
  zero overhead.
* Program data/arrays/buffers are variables (`≥0x4000`, in WIN2) and pass
  directly; the bounce only triggers for WIN0 pointers (inline literals, the
  command-line string).

In other words, an existing source builds in the ~47 KB layout by swapping its
Makefile's `include` line for `include ../win0.mk`.

## 11.9. Runtime files (SDK)

* `lib/win0/crt0_win0.s` — payload startup (entry `0x0180`).
* `lib/win0/win0_rt.s` — `_WINRT`@`0x8000`: RST trampolines + vector installer +
  page table (`_wrt_dss/p0/p1/p2`).
* `lib/win0/win0_dss.c` — overriding bounce wrappers for the pointer-passing DSS
  calls (`puts/open/creat/read/write/curdir/delete/rename/chdir/mkdir/rmdir/
  ffirst/fnext/getenv/setenv/expath/exec`).
* `lib/win0/dss_raw.s` — the raw `dss_*_raw` (RST #10 without bounce).
* `lib/win0/loader.c` — stage-1 PRELOAD loader.
* `tools/win0_exe.py` — `.EXE` packager.
* `examples/win0.mk` — the build profile.

## 11.10. Notes and edge cases

* **Nested `dss_exec`** works: the child runs in the normal layout; on exit DSS
  restores the parent's P1/P2 and the RST10 trampoline restores WIN0=P0, so the
  parent (and its vectors) resume. The command literal is bounced.
* **No page leak at exit:** `DSS.Exit` frees the pages the process GETMEM'd
  (P0/P1/P2). Verified by comparing free pages across runs.
* `gfx.lib`/VRAM (pages `#50+`) use ports/WIN3 and do not conflict.
* Self-modifying code / compiler jump tables in `_CODE` (WIN0/WIN1) execute
  normally — WIN0 is only repaged during a firmware call, when execution is
  already inside the WIN2 trampoline.

## 11.11. Examples

Usage examples (one-line Makefile `include ../win0.mk`):

* `35_win0app` — the profile + a private full WIN2 (an 8 KB global);
* `37_win0big` — multi-page: code/rodata spanning WIN0+WIN1;
* `38_win0printf` — `printf`, unchanged;
* `39_win0args` — command line (`dss_cmdline()`);
* `40_win0file` — stdio (`fopen`/`fputs`/`fgets`), unchanged;
* `41_win0misc` — directory ops (`mkdir/chdir/rmdir/ffirst`) + a page-leak check;
* `42_win0exec` — nested `dss_exec` (a win0 parent launches a standard child).
