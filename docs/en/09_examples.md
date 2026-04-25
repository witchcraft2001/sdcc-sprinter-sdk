# Examples

The SDK includes 21 example programs demonstrating various SDK features. All examples are in the `examples/` directory, each with its own `main.c` and `Makefile`.

## Building Examples

Build all examples at once:

```bash
make examples
```

Build a single example:

```bash
cd examples/01_hello
make
```

Each example produces an `.exe` file (e.g., `hello.exe`) that can be copied to the Sprinter and run from the DSS command line.

## Example List

### 01_hello -- Hello World (681 B)

The simplest possible Sprinter program. Uses `dss_puts()` for output and `dss_waitkey()` to pause.

```c
#include <sprinter.h>

void main(void) {
    dss_puts("Hello from ZX Sprinter!\r\n");
    dss_puts("SDCC SDK v1.0\r\n");
    dss_puts("\r\nPress any key to exit...\r\n");
    dss_waitkey();
}
```

**Demonstrates:** minimal program structure, `<sprinter.h>`, DSS console output.

### 02_files -- Low-Level File I/O (1.4 KB)

Creates a file, writes data, reads it back, and deletes it using DSS file functions.

**Demonstrates:** `dss_creat()`, `dss_write()`, `dss_open()`, `dss_read()`, `dss_close()`, `dss_delete()`.

### 03_graphics -- Graphics Mode (853 B)

Switches to 320x256, 256-color mode. Sets the GRAF palette via BIOS #A6. Draws palette bars, a white frame, and a diagonal color gradient.

**Demonstrates:** `video_setmode()`, `bios_putpixel()`, BIOS palette preset, `video_safe_porty()`, inline assembly.

### 04_mouse -- Mouse Input (1.3 KB)

Initializes the mouse driver, shows the cursor, and tracks position/buttons in real-time. Exit by pressing a key or right-clicking.

**Demonstrates:** `mouse_init()`, `mouse_show()`, `mouse_hide()`, `mouse_stat()`, `mouse_xbound()`, `mouse_ybound()`, `mouse_state_t`, `dss_kbhit()`.

### 05_keyinput -- System Info and Keyboard (1.4 KB)

Displays BIOS version, board ID, current disk, date/time, then enters a keyboard echo loop. Press ESC to exit.

**Demonstrates:** `bios_version()`, `bios_board_id()`, `dss_getdisk()`, `dss_getdate()`, `dss_gettime()`, `dss_waitkey()`, keyboard handling.

### 06_printf -- printf Demo (2.8 KB)

Demonstrates `printf()` format specifiers: `%d`, `%X`, `%s`.

```c
#include <stdio.h>

void main(void) {
    int num = 2004;
    printf("Hello world!\n");
    printf("Year: %d\n", num);
    printf("Hex: 0x%X\n", num);
    printf("String: %s\n", "test");
    puts("\nPress any key...");
    getchar();
}
```

**Demonstrates:** `<stdio.h>`, `printf()`, `puts()`, `getchar()`. Shows the code size impact of the printf engine.

### 07_fileio -- Standard File I/O (3.7 KB)

Creates a file using `fopen("w")`, writes with `fwrite()`, reads back with `fread()`, then deletes with `remove()`.

**Demonstrates:** `fopen()`, `fclose()`, `fread()`, `fwrite()`, `remove()`, `<string.h>` (`strlen`, `memset`).

### 08_sort -- Sorting Algorithms (4.3 KB)

Implements Bubble Sort, Shell Sort, and Quick Sort. Fills arrays with random numbers, sorts them, and prints statistics (iterations, exchanges, conditions). Ported from SOLID C's SORT2.C.

**Demonstrates:** `printf()`, `cprintf()`, `cputs()`, `rand()`, `srand()`, `getch()`, recursion (quicksort), porting from SOLID C.

### 09_dirlist -- Directory Listing (2.9 KB)

Lists all files in the current directory using `dss_ffirst()` / `dss_fnext()`.

```c
#include <stdio.h>
#include <sprinter/dss.h>

void main(void) {
    dss_find_t entry;
    printf("Directory listing: *.*\n\n");
    if (dss_ffirst("*.*", &entry, 0x20) == 0) {
        do {
            printf("%s\n", entry.ff_name);
        } while (dss_fnext(&entry) == 0);
    }
    printf("\nDone. Press any key.\n");
    getchar();
}
```

**Demonstrates:** `dss_ffirst()`, `dss_fnext()`, `dss_find_t`, mixing standard and Sprinter APIs.

### 10_bin2c -- Binary to C Array (3.6 KB)

Reads a binary file and outputs its contents as a C array declaration (`unsigned char data[] = { 0x00, 0x01, ... };`). Useful for embedding binary data in C programs.

**Demonstrates:** `fopen()`, `fgetc()`, `printf()` formatting, interactive input with `getchar()`.

### 11_fprintf -- fprintf to File (3.6 KB)

Writes formatted text to a file using `fprintf()`, then reads it back with `fgets()` and displays it.

**Demonstrates:** `fprintf()`, `fgets()`, file creation and reading, `remove()`.

### 12_strings -- String and Character Functions (3.6 KB)

Exercises `string.h` functions (`strlen`, `strcmp`, `strcpy`, `strcat`, `strchr`) and `ctype.h` functions (`isalpha`, `isdigit`, `isupper`, `toupper`).

**Demonstrates:** `<string.h>`, `<ctype.h>`, all major string operations.

### 13_random -- Random Numbers and stdlib (3.3 KB)

Demonstrates `stdlib.h` functions: `abs()`, `atoi()`, `rand()`, `srand()`. Generates and displays random numbers with different seeds.

**Demonstrates:** `<stdlib.h>`, `rand()`, `srand()`, `atoi()`, `abs()`.

### 14_bench -- Cross-Compiler Benchmark (4.7 KB)

Runs a benchmark suite ported from SOLID C (sieve, sort, CRC-16, RC4 and others) to compare runtime and code size.

**Demonstrates:** larger multi-function C application, performance-sensitive code paths, SOLID C compatibility patterns.

### 15_exec -- EXEC and Error Reporting (~3 KB)

Launches another `.EXE` with `dss_exec_ex()` and prints either child exit code or raw DSS launch error code.

```c
#include <stdio.h>
#include <sprinter.h>

void main(void) {
    u8 err;
    i16 rc = dss_exec_ex("CHILD.EXE arg1", &err);
    if (rc < 0)
        printf("EXEC failed: A=0x%X (%u)\n", (u16)err, (u16)err);
    else
        printf("Child exit code: %d\n", rc);
}
```

**Demonstrates:** `dss_exec_ex()`, DSS `EXEC` error handling, interpreting child exit status.

### 16_appinfo -- APPINFO and Path Parsing

Reads `APPINFO` fields for the running program, shows `getcwd()`, and splits the executable path with both `fnsplit()` and `dss_expath()`.

**Demonstrates:** `dss_appinfo()`, `dss_expath()`, `getcwd()`, `fnsplit()`.

### 17_envdir -- Environment and Directory Operations

Creates and removes a temporary directory, changes into it and back, and sets/reads/deletes an environment variable.

**Demonstrates:** `putenv()`, `getenv()`, `mkdir()`, `chdir()`, `rmdir()`, `getcwd()`.

### 18_call -- CALL and CALLP

Calls local C functions indirectly through `dss_call()` and `dss_callp()` and prints the returned values.

**Demonstrates:** `dss_call()`, `dss_callp()`, calling plain C functions through DSS trampolines.

### 19_console -- Console Window and Video State

Reads the current DSS video mode/page, reapplies the same mode, clears a text window and scrolls it.

**Demonstrates:** `dss_getvmod()`, `dss_setvmod()`, `dss_clear()`, `dss_scroll()`, `dss_gotoxy()`.

### 20_gfxst -- Static gfx.lib Sprites (~2 KB)

Demonstrates the separate `gfx.lib` archive: fixed 8x8, 16x16, and 24x24 sprites, transparent color `0xFF`, drawing to both graphics screens, `GFX_VRAM_ONLY`, and background restore from the shadow page.

Expected output: a black graphics screen with a white border, a red filled rectangle in the lower-left area, a thin gray diagonal line, a thick blue diagonal line, a white circle in the lower-right area, and four small X/cross sprites. After the first key press, the rightmost temporary sprite is restored from the shadow page; after the second key press, the example returns to text mode.

**Demonstrates:** `gfx_draw_sprite8()`, `gfx_draw_sprite16()`, `gfx_draw_sprite24()`, `gfx_draw_line()`, `gfx_draw_line_thick()`, `gfx_draw_rect()`, `gfx_fill_rect()`, `gfx_draw_circle()`, `gfx_restore_sprite16()`, `video_setpal_range()`.

### 21_gfx_resources -- PNG gfx.lib Resources (~3 KB + `.gfx`)

Shows resource preparation through `tools/png2gfx.py`, generated `.gfx` and `res.h`, loading the file into paged memory, installing the shared palette with `video_setpal_range()`, and drawing resources by id.

Expected output: a black graphics screen with two X/cross sprites loaded from `GFXDEMO.GFX`. The example flips to the copied graphics screen after drawing, then flips back on the next key press.

**Demonstrates:** `gfx_load_resource_pages()`, `gfx_draw_resource()`, `gfx_resource_t`, generated PNG resources, external `EXTRA_LIBS`.

## Creating Your Own Example

### Target Filename Limits

DSS does not support long filenames. Any file copied to the target floppy image must use a DOS 8.3 name: up to 8 characters for the base name and up to 3 for the extension. This includes example executables, `.gfx` resources, generated assets, and any data files opened by the program. `scripts/make_floppy.sh` checks deployable `.exe` and `.gfx` files and fails if a long filename is found.

1. Create a new directory:

```bash
mkdir examples/20_myproject
```

2. Create `main.c`:

```c
#include <stdio.h>

void main(void) {
    printf("My project works!\n");
    getchar();
}
```

3. Create a `Makefile` with three lines:

```makefile
APP  = myproj
SRCS = main.c
include ../common.mk
```

4. Build:

```bash
cd examples/20_myproject
make
```

5. The result is `myproj.exe`, ready to run on the Sprinter.

### Using an External Project Directory

You can place your project anywhere on disk. Just point `SDK_DIR` to the SDK root:

```makefile
APP      = myapp
SRCS     = main.c render.c input.c
SDK_DIR  = /Users/me/sdcc-sprinter-sdk/
include $(SDK_DIR)examples/common.mk
```

### Adding Assembly Files

For mixed C+ASM projects, compile `.s` files separately and add them as extra `.rel` files. You will need to extend the Makefile beyond `common.mk` for this. See the sjasmplus documentation and SDCC manual for details on mixing C and assembly.
