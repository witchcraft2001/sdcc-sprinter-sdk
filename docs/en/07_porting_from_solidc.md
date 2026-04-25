# Porting from SOLID C

This chapter provides a detailed guide for converting SOLID C (Sprinter's original C compiler) programs to the SDCC Sprinter SDK. Many SOLID C programs can be ported with straightforward, mechanical changes.

## Language Differences

### K&R Function Declarations to ANSI

SOLID C supports K&R-style (old-style) function declarations. SDCC requires ANSI-style declarations.

**SOLID C:**

```c
void func(a, b)
int a, b;
{
    /* ... */
}
```

**SDCC SDK:**

```c
void func(int a, int b) {
    /* ... */
}
```

### main() Signature

**SOLID C:**

```c
main()
{
    /* ... */
}
```

**SDCC SDK:**

```c
void main(void) {
    /* ... */
}
```

SDCC requires an explicit return type. Use `void main(void)` -- DSS does not use the return value of main (the CRT0 passes L register to DSS.Exit).

### Remove #pragma nonrec and recursive Keywords

SOLID C requires `#pragma nonrec` for non-recursive functions (optimization) and the `recursive` keyword for recursive ones. SDCC handles stack frame allocation automatically -- simply remove these directives.

**SOLID C:**

```c
#pragma nonrec

int add(a, b)
int a, b;
{
    return a + b;
}

recursive int factorial(n)
int n;
{
    if (n <= 1) return 1;
    return n * factorial(n - 1);
}
```

**SDCC SDK:**

```c
int add(int a, int b) {
    return a + b;
}

int factorial(int n) {
    if (n <= 1) return 1;
    return n * factorial(n - 1);
}
```

### Variadic Functions

SOLID C uses a dot `.` notation for variable-argument functions. SDCC uses standard `<stdarg.h>`.

**SOLID C:**

```c
int myprintf(fmt, .)
char *fmt;
{
    /* access args via stack pointer manipulation */
}
```

**SDCC SDK:**

```c
#include <stdarg.h>

int myprintf(const char *fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    /* use va_arg(ap, type) to get each argument */
    va_end(ap);
    return 0;
}
```

## Type Differences

| SOLID C Type | SDCC SDK Equivalent | Notes |
|-------------|---------------------|-------|
| `TINY` | `char` or `i8` | 8-bit signed |
| `BOOL` | `bool` or `u8` | Use `<stdbool.h>` for `bool` |
| `BYTE` | `u8` or `unsigned char` | 8-bit unsigned |
| `WORD` | `u16` or `unsigned int` | 16-bit unsigned |
| `uint` | `unsigned int` | SDCC does not define `uint`; spell it out |
| `FD` | `i16` or `int` | File descriptor |
| `f_point` | `u32` or `long` | SOLID used a struct with high/low words |

### f_point Struct

SOLID C represented 32-bit file positions as a struct with two 16-bit words:

**SOLID C:**

```c
typedef struct {
    unsigned high;
    unsigned low;
} f_point;

f_point pos;
lseek(fd, &pos, SEEK_SET);
```

**SDCC SDK:**

```c
u32 offset = 0x10000UL;   /* or use 'long' */
dss_seek(fd, offset, SEEK_SET);
```

The SDK uses native `u32` / `long` for 32-bit values.

## Header Mapping

### Fully Compatible Headers

These headers work the same way in SOLID C and the SDCC SDK. In many cases, no code changes are needed:

| SOLID C | SDCC SDK | Functions |
|---------|----------|-----------|
| `<stdio.h>` | `<stdio.h>` | `printf`, `sprintf`, `fprintf`, `puts`, `putchar`, `getchar`, `fopen`, `fclose`, `fread`, `fwrite`, `fgets`, `fputs`, `fseek`, `remove` |
| `<conio.h>` | `<conio.h>` | `getch`, `getche`, `kbhit`, `clrscr`, `gotoxy`, `cputs`, `cprintf`, `putch` |
| `<stdlib.h>` | `<stdlib.h>` | `rand`, `srand`, `atoi`, `abs`, `exit` |
| `<string.h>` | `<string.h>` | `strlen`, `strcpy`, `strncpy`, `strcmp`, `strncmp`, `strcat`, `strncat`, `strchr`, `strrchr`, `memcpy`, `memset`, `memcmp` |

### Headers That Need Replacement

| SOLID C | SDCC SDK | Migration Notes |
|---------|----------|-----------------|
| `<dos.h>` | `<sprinter/dss.h>` | Functions renamed: `ffirst` -> `dss_ffirst`, `fnext` -> `dss_fnext`, `bdos` -> use specific `dss_*` functions |
| `<io.h>` | `<sprinter/dss.h>` | `open` -> `dss_open`, `close` -> `dss_close`, `read` -> `dss_read`, `write` -> `dss_write`, `lseek` -> `dss_seek` |
| `<mouse.h>` | `<sprinter/mouse.h>` | API restructured; see mouse function mapping below |
| `<types.h>` | `<sprinter/types.h>` | `BYTE`/`WORD`/`TINY`/`BOOL` -> `u8`/`u16`/`i8`/`bool` |

### Headers Not Available

| SOLID C | SDCC SDK | Workaround |
|---------|----------|------------|
| `<malloc.h>` | Not available | Use static arrays, or `dss_getmem()` / `dss_freemem()` for page-level allocation |
| `<errno.h>` | Not available | Check return values directly (-1 for errors) |
| `<setjmp.h>` | Not available | Restructure code to avoid `setjmp`/`longjmp` |

## Function Mapping

### Console Functions (work as-is)

| SOLID C | SDCC SDK | Header |
|---------|----------|--------|
| `printf(fmt, ...)` | `printf(fmt, ...)` | `<stdio.h>` |
| `sprintf(buf, fmt, ...)` | `sprintf(buf, fmt, ...)` | `<stdio.h>` |
| `fprintf(fp, fmt, ...)` | `fprintf(fp, fmt, ...)` | `<stdio.h>` |
| `puts(s)` | `puts(s)` | `<stdio.h>` |
| `getchar()` | `getchar()` | `<stdio.h>` |
| `getch()` | `getch()` | `<conio.h>` |
| `kbhit()` | `kbhit()` | `<conio.h>` |
| `clrscr()` | `clrscr()` | `<conio.h>` |
| `gotoxy(x, y)` | `gotoxy(x, y)` | `<conio.h>` |
| `cputs(s)` | `cputs(s)` | `<conio.h>` |

### Standard File I/O (work as-is)

| SOLID C | SDCC SDK | Header |
|---------|----------|--------|
| `fopen(path, mode)` | `fopen(path, mode)` | `<stdio.h>` |
| `fclose(fp)` | `fclose(fp)` | `<stdio.h>` |
| `fread(buf, sz, cnt, fp)` | `fread(buf, sz, cnt, fp)` | `<stdio.h>` |
| `fwrite(buf, sz, cnt, fp)` | `fwrite(buf, sz, cnt, fp)` | `<stdio.h>` |
| `fgets(buf, n, fp)` | `fgets(buf, n, fp)` | `<stdio.h>` |
| `fseek(fp, off, whence)` | `fseek(fp, off, whence)` | `<stdio.h>` |
| `remove(path)` | `remove(path)` | `<stdio.h>` |

### Low-Level File I/O (renamed)

| SOLID C (`<io.h>`) | SDCC SDK (`<sprinter/dss.h>`) | Notes |
|---------------------|-------------------------------|-------|
| `open(path, mode)` | `dss_open(path, mode)` | Mode constants are compatible |
| `close(fd)` | `dss_close(fd)` | Parameter is `u8` in SDK |
| `read(fd, buf, n)` | `dss_read(fd, buf, n)` | Returns `i16` |
| `write(fd, buf, n)` | `dss_write(fd, buf, n)` | Returns `i16` |
| `lseek(fd, &pos, whence)` | `dss_seek(fd, offset, whence)` | Uses `u32` offset, not struct |

### DOS Functions (renamed)

| SOLID C (`<dos.h>`) | SDCC SDK | Notes |
|---------------------|----------|-------|
| `ffirst(pattern, &result, attr)` | `dss_ffirst(pattern, &result, attr)` | Different struct: `FIND` -> `dss_find_t` |
| `fnext(&result)` | `dss_fnext(&result)` | Uses `dss_find_t` |
| `bdos(func, ...)` | Use specific `dss_*` functions | No generic `bdos()` call |
| `intdos(func, ...)` | Use specific `dss_*` functions | No generic dispatcher |
| `chdir(path)` | `dss_chdir(path)` | |
| `enable()` | `dss_ei()` | Enable interrupts |
| `disable()` | `dss_di()` | Disable interrupts |

### Port I/O (compatible)

| SOLID C | SDCC SDK | Header |
|---------|----------|--------|
| `inp(port)` | `inp(port)` | `<sprinter/bios.h>` |
| `outp(port, val)` | `outp(port, val)` | `<sprinter/bios.h>` |

### Mouse Functions (restructured)

| SOLID C (`<mouse.h>`) | SDCC SDK (`<sprinter/mouse.h>`) | Notes |
|-----------------------|----------------------------------|-------|
| `ms_init()` | `mouse_init()` | Returns `u8` (1=present) |
| `ms_show()` | `mouse_show()` | |
| `ms_hide()` | `mouse_hide()` | |
| `ms_stat(&x, &y, &btn)` | `mouse_stat(&state)` | Uses `mouse_state_t` struct |
| `ms_setpos(x, y)` | `mouse_setpos(x, y)` | |
| `ms_xbound(min, max)` | `mouse_xbound(min, max)` | |
| `ms_ybound(min, max)` | `mouse_ybound(min, max)` | |

The SOLID C mouse API passed x, y, and buttons as separate pointer parameters. The SDK uses a `mouse_state_t` struct:

**SOLID C:**

```c
int x, y, buttons;
ms_stat(&x, &y, &buttons);
if (buttons & 1) { /* left button */ }
```

**SDCC SDK:**

```c
mouse_state_t ms;
mouse_stat(&ms);
if (ms.buttons & MS_BTN_LEFT) { /* left button */ }
```

### Directory Search Struct

The `FIND` / `ffblk` struct from SOLID C is replaced by `dss_find_t`:

**SOLID C:**

```c
#include <dos.h>

struct FIND info;
if (ffirst("*.*", &info, 0x20) == 0) {
    do {
        printf("%s\n", info.ff_name);
    } while (fnext(&info) == 0);
}
```

**SDCC SDK:**

```c
#include <sprinter/dss.h>

dss_find_t entry;
if (dss_ffirst("*.*", &entry, 0x20) == 0) {
    do {
        printf("%s\n", entry.ff_name);
    } while (dss_fnext(&entry) == 0);
}
```

The `ff_name` field is compatible -- it contains the null-terminated filename.

## Calling Convention Differences

SOLID C passes parameters in Z80 registers (HL, DE, BC, A) in a custom convention. This SDK targets SDCC 2.9.0, whose default ABI is stack-based for most C calls and differs significantly from SOLID C:

- 1st parameter: `u8` in A, `u16`/pointer in HL
- 2nd parameter: `u8` in L (if 1st in A), `u16`/pointer in DE
- Return value: `u8` in A, `u16`/pointer in DE

**You do not need to worry about this for normal C code.** The calling convention only matters if you are writing inline assembly or linking with external ASM modules written for SOLID C. In that case, the assembly code must be adapted to match SDCC's convention.

## Step-by-Step Example: Porting SORT2.C

Here is a real example of porting SOLID C's `SORT2.C` (sorting algorithms demo) to the SDCC SDK.

### Original SOLID C Code (excerpts)

```c
/* SORT2.C — SOLID C for ZX Sprinter */
#include <stdio.h>
#include <stdlib.h>
#include <conio.h>

#pragma nonrec

#define SIZE 15

unsigned iterations;
unsigned exchanges;
unsigned conditions;

void print_array(arr, count)
int *arr, count;
{
    int i;
    cprintf("[");
    for (i = 0; i < count; i++)
        cprintf(" %3d", arr[i]);
    cputs(" ]\n");
}

void bubble_sort(arr, count)
int *arr, count;
{
    int i, j;
    for (i = 0; i < count; i++)
        for (j = i + 1; j < count; j++) {
            if (arr[i] > arr[j]) {
                int tmp = arr[i];
                arr[i] = arr[j];
                arr[j] = tmp;
                exchanges++;
                conditions++;
            }
            iterations++;
        }
}

recursive void quick_sort(arr, left, right)
int *arr, left, right;
{
    int x, y, i, j;
    /* ... recursive quicksort ... */
    if (left < j)  quick_sort(arr, left, j);
    if (i < right) quick_sort(arr, i, right);
}

main()
{
    int arr[SIZE];
    srand(12345);
    /* ... */
    getch();
}
```

### Ported SDCC SDK Code

```c
/* 08_sort — Sorting algorithms demo
 * Ported from SOLID SORT2.C
 */
#include <stdio.h>
#include <stdlib.h>
#include <conio.h>

#define SIZE 15

/* 1. Removed: #pragma nonrec */

unsigned int iterations;     /* 2. 'unsigned' -> 'unsigned int' (explicit) */
unsigned int exchanges;
unsigned int conditions;

/* 3. K&R declarations -> ANSI */
void print_array(int *arr, int count) {
    int i;
    cprintf("[");
    for (i = 0; i < count; i++)
        cprintf(" %3d", arr[i]);
    cputs(" ]\n");
}

/* 3. K&R -> ANSI */
void bubble_sort(int *arr, int count) {
    int i, j;
    for (i = 0; i < count; i++)
        for (j = i + 1; j < count; j++) {
            if (arr[i] > arr[j]) {
                int tmp = arr[i];
                arr[i] = arr[j];
                arr[j] = tmp;
                exchanges++;
                conditions++;
            }
            iterations++;
        }
}

/* 4. Removed: 'recursive' keyword; K&R -> ANSI */
void quick_sort(int *arr, int left, int right) {
    int x, y, i, j;
    /* ... same logic, no changes needed ... */
    if (left < j)  quick_sort(arr, left, j);
    if (i < right) quick_sort(arr, i, right);
}

/* 5. main() -> void main(void) */
void main(void) {
    int arr[SIZE];

    printf("=== Sorting Algorithms Demo ===\n");
    printf("Array size: %d elements\n", SIZE);

    srand(12345);

    /* run_sort calls -- logic unchanged */
    /* ... */

    printf("\nDone! Press any key.\n");
    getch();
}
```

### Summary of Changes

| # | Change | Description |
|---|--------|-------------|
| 1 | Remove `#pragma nonrec` | SDCC handles this automatically |
| 2 | `unsigned` -> `unsigned int` | Explicit type for clarity |
| 3 | K&R function declarations | Convert to ANSI style |
| 4 | Remove `recursive` keyword | SDCC supports recursion by default |
| 5 | `main()` -> `void main(void)` | Explicit return type and parameter list |

The headers (`stdio.h`, `stdlib.h`, `conio.h`) and functions (`printf`, `cprintf`, `cputs`, `getch`, `srand`, `rand`) work without changes. The core algorithm logic is completely unchanged.

## Porting Checklist

When porting a SOLID C program, work through this checklist:

1. **Headers:** Replace `<dos.h>` with `<sprinter/dss.h>`, `<io.h>` with `<sprinter/dss.h>`, `<mouse.h>` with `<sprinter/mouse.h>`, `<types.h>` with `<sprinter/types.h>`. Remove `<malloc.h>`, `<errno.h>`, `<setjmp.h>` (not available).

2. **Function signatures:** Convert all K&R declarations to ANSI. Add `void` to empty parameter lists.

3. **main():** Change to `void main(void)`.

4. **Pragmas:** Remove `#pragma nonrec`. Remove `recursive` keyword from function declarations.

5. **Types:** Replace `TINY`/`BOOL` with `char`/`bool`, `BYTE` with `u8`, `WORD` with `u16`, `uint` with `unsigned int`, `FD` with `i16`, `f_point` with `u32`.

6. **Low-level I/O:** Replace `open`/`close`/`read`/`write`/`lseek` with `dss_open`/`dss_close`/`dss_read`/`dss_write`/`dss_seek`.

7. **DOS functions:** Replace `ffirst`/`fnext` with `dss_ffirst`/`dss_fnext`. Replace `FIND` struct with `dss_find_t`. Replace `bdos`/`intdos` with specific `dss_*` calls.

8. **Mouse:** Replace `ms_*` calls with `mouse_*` calls. Adapt to `mouse_state_t` struct.

9. **Variadic functions:** Replace `.` notation with `<stdarg.h>` and `...`.

10. **Test:** Build with `make` and test on emulator. Fix any remaining type warnings.
