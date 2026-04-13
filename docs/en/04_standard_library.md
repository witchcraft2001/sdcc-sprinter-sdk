# Standard Library Reference

## Overview

The SDK provides standard C headers that map to DSS OS calls internally. This makes it easy to write portable C programs or port existing code to the Sprinter.

| Header | Description | Modules |
|--------|-------------|---------|
| `<stdio.h>` | File and console I/O, printf | 20 |
| `<stdlib.h>` | exit, atoi, abs, rand/srand | 4 |
| `<string.h>` | String and memory operations | 12 |
| `<ctype.h>` | Character classification and conversion | 12 |
| `<conio.h>` | Console I/O (CP/M / Turbo C style) | 8 |
| `<stddef.h>` | `size_t`, `NULL`, `offsetof` | (header only) |
| `<stdbool.h>` | `bool`, `true`, `false` | (header only) |

All functions use selective linking -- only the functions your program calls are included in the final binary.

## stdio.h -- Standard I/O

### FILE Structure

```c
typedef struct _FILE {
    unsigned char fd;           /* DSS file handle (0-9), 0xFF = console */
    unsigned char flags;        /* _F_READ, _F_WRITE, _F_EOF, _F_ERR, _F_OPEN */
    unsigned char ungetc_buf;   /* ungetc buffer, 0xFF = empty */
} FILE;
```

The SDK supports up to `FOPEN_MAX` (10) simultaneously open files.

### Standard Streams

```c
extern FILE *stdin;     /* Console input */
extern FILE *stdout;    /* Console output */
extern FILE *stderr;    /* Console output (same as stdout) */
```

All three streams are mapped to the DSS console. `printf()` writes to `stdout`, `getchar()` reads from `stdin`.

### File Operations

```c
FILE *fopen(const char *path, const char *mode);
int   fclose(FILE *fp);
size_t fread(void *buf, size_t size, size_t count, FILE *fp);
size_t fwrite(const void *buf, size_t size, size_t count, FILE *fp);
int   fseek(FILE *fp, long offset, int whence);
long  ftell(FILE *fp);
void  rewind(FILE *fp);
int   feof(FILE *fp);
int   ferror(FILE *fp);
int   remove(const char *path);
int   rename(const char *oldpath, const char *newpath);
```

**Supported fopen modes:**

| Mode | Description |
|------|-------------|
| `"r"` | Open for reading |
| `"w"` | Create/truncate for writing |
| `"a"` | Open/create for appending |
| `"r+"` | Open for reading and writing |
| `"w+"` | Create/truncate for reading and writing |

Binary mode (`"rb"`, `"wb"`) is accepted but has no effect -- all I/O is binary on DSS.

**Seek origins:** `SEEK_SET` (0), `SEEK_CUR` (1), `SEEK_END` (2).

**Example:**

```c
#include <stdio.h>
#include <string.h>

void main(void) {
    FILE *fp;
    char buf[64];
    const char *msg = "Hello from stdio!\n";

    fp = fopen("TEST.TXT", "w");
    if (!fp) { printf("Error creating file\n"); return; }
    fwrite(msg, 1, strlen(msg), fp);
    fclose(fp);

    fp = fopen("TEST.TXT", "r");
    if (!fp) { printf("Error opening file\n"); return; }
    memset(buf, 0, sizeof(buf));
    fread(buf, 1, 63, fp);
    fclose(fp);

    printf("Read back: %s\n", buf);
    remove("TEST.TXT");
}
```

### Character I/O

```c
int   fgetc(FILE *fp);          /* Read one character (returns EOF on end) */
int   fputc(int c, FILE *fp);   /* Write one character */
int   fputs(const char *s, FILE *fp);   /* Write string to stream */
char *fgets(char *buf, int size, FILE *fp);  /* Read line (up to size-1 chars) */
int   ungetc(int c, FILE *fp);  /* Push character back */
```

### Console I/O

```c
int putchar(int c);      /* Print character to stdout */
int puts(const char *s); /* Print string + newline to stdout */
int getchar(void);       /* Read character from stdin (blocking) */
```

### Formatted I/O (printf Family)

```c
int printf(const char *fmt, ...);
int sprintf(char *buf, const char *fmt, ...);
int fprintf(FILE *fp, const char *fmt, ...);
```

**Supported format specifiers:**

| Specifier | Description |
|-----------|-------------|
| `%d`, `%i` | Signed decimal integer |
| `%u` | Unsigned decimal integer |
| `%x` | Unsigned hexadecimal (lowercase) |
| `%X` | Unsigned hexadecimal (uppercase) |
| `%s` | String |
| `%c` | Character |
| `%%` | Literal percent sign |

**Supported modifiers:**

| Modifier | Example | Description |
|----------|---------|-------------|
| Field width | `%5d` | Minimum width, right-aligned |
| Zero-pad | `%04X` | Pad with zeros |
| Left-align | `%-10s` | Left-align within field width |
| `l` modifier | `%ld` | Accepted for compatibility, but integer conversions are still 16-bit |

**NOT supported:**

- Floating point (`%f`, `%e`, `%g`) -- Z80 has no FPU, and float support would add significant code size
- `%ld` does not actually print 32-bit values -- the `l` modifier is parsed but ignored; the value is treated as 16-bit
- `%o` (octal) is not implemented
- Precision (`.5`) is not implemented

**Example:**

```c
#include <stdio.h>

void main(void) {
    int num = 2004;
    printf("Decimal: %d\n", num);
    printf("Hex: 0x%04X\n", num);
    printf("String: [%-10s]\n", "test");
    printf("Char: %c\n", 'A');
}
```

**Output:**

```
Decimal: 2004
Hex: 0x07D4
String: [test      ]
Char: A
```

## stdlib.h -- Utility Functions

```c
#define EXIT_SUCCESS 0
#define EXIT_FAILURE 1
#define RAND_MAX 32767

void exit(int status);              /* Exit program with status code */
int  atoi(const char *s);           /* Convert string to integer */
int  abs(int n);                    /* Absolute value */
int  rand(void);                    /* Random number 0..RAND_MAX */
void srand(unsigned int seed);      /* Seed random number generator */
```

**Example:**

```c
#include <stdio.h>
#include <stdlib.h>

void main(void) {
    srand(42);
    printf("Random: %d\n", rand() % 100);
    printf("atoi: %d\n", atoi("12345"));
    printf("abs(-7): %d\n", abs(-7));
}
```

## string.h -- String Operations

```c
size_t strlen(const char *s);
char  *strcpy(char *dest, const char *src);
char  *strncpy(char *dest, const char *src, size_t n);
int    strcmp(const char *s1, const char *s2);
int    strncmp(const char *s1, const char *s2, size_t n);
char  *strcat(char *dest, const char *src);
char  *strncat(char *dest, const char *src, size_t n);
char  *strchr(const char *s, int c);
char  *strrchr(const char *s, int c);
void  *memcpy(void *dest, const void *src, size_t n);
void  *memset(void *s, int c, size_t n);
int    memcmp(const void *s1, const void *s2, size_t n);
```

All 12 functions behave identically to their ANSI C counterparts.

**Example:**

```c
#include <stdio.h>
#include <string.h>

void main(void) {
    char buf[64];
    strcpy(buf, "Hello");
    strcat(buf, ", World!");
    printf("%s (len=%d)\n", buf, strlen(buf));
    printf("strcmp: %d\n", strcmp("abc", "abd"));
}
```

## ctype.h -- Character Classification

```c
int isalpha(int c);     /* Is alphabetic (A-Z, a-z) */
int isdigit(int c);     /* Is digit (0-9) */
int isalnum(int c);     /* Is alphanumeric */
int isspace(int c);     /* Is whitespace (space, tab, newline, etc.) */
int isupper(int c);     /* Is uppercase letter */
int islower(int c);     /* Is lowercase letter */
int isprint(int c);     /* Is printable (including space) */
int isxdigit(int c);    /* Is hexadecimal digit (0-9, A-F, a-f) */
int iscntrl(int c);     /* Is control character */
int ispunct(int c);     /* Is punctuation */
int toupper(int c);     /* Convert to uppercase */
int tolower(int c);     /* Convert to lowercase */
```

All 12 functions behave identically to their ANSI C counterparts.

**Example:**

```c
#include <stdio.h>
#include <ctype.h>

void str_toupper(char *s) {
    while (*s) { *s = toupper(*s); s++; }
}

void main(void) {
    char buf[] = "Hello, World!";
    str_toupper(buf);
    printf("%s\n", buf);  /* HELLO, WORLD! */
}
```

## conio.h -- Console I/O

These functions provide a familiar console I/O interface compatible with Turbo C / Borland C / CP/M:

```c
int  getch(void);               /* Read key, no echo (blocking) */
int  getche(void);              /* Read key with echo (blocking) */
int  kbhit(void);               /* Check if key is available (non-blocking) */
int  putch(int c);              /* Output character */
void cputs(const char *s);      /* Output string */
int  cprintf(const char *fmt, ...);  /* Formatted console output */
void clrscr(void);              /* Clear screen */
void gotoxy(int x, int y);     /* Set cursor position (1-based) */
```

**Notes:**
- `gotoxy()` uses 1-based coordinates: `gotoxy(1, 1)` is the top-left corner.
- `cprintf()` supports the same format specifiers as `printf()`.
- `cputs()` does not add a newline (unlike `puts()`).

**Example:**

```c
#include <conio.h>

void main(void) {
    clrscr();
    gotoxy(10, 5);
    cputs("Press any key...");
    getch();
    clrscr();
}
```

## Important Limitations

1. **No `malloc()` / `free()`** -- dynamic memory allocation is not yet implemented. Use static arrays or DSS page allocation (`dss_getmem()` / `dss_freemem()`).

2. **No floating point** -- the Z80 has no FPU. The `float` and `double` types are not supported. Use fixed-point arithmetic or integer scaling.

3. **Integers are 16-bit** -- `int` is 16 bits (-32768 to 32767), `unsigned int` is 16 bits (0 to 65535). Use `long` (32-bit) for larger values, but be aware that `printf("%ld", ...)` does not actually print 32-bit values.

4. **No `scanf()`** -- use `getchar()` and `atoi()` for input parsing.
