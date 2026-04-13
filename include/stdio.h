/**
 * stdio.h — Standard I/O for ZX Sprinter SDK
 *
 * Minimal but functional subset of ANSI C stdio.
 * Maps to DSS OS calls internally.
 */

#ifndef _STDIO_H
#define _STDIO_H

#include <stddef.h>

/* ===== Constants ===== */
#define EOF         (-1)
#define FOPEN_MAX   10

#ifndef SEEK_SET
#define SEEK_SET    0
#define SEEK_CUR    1
#define SEEK_END    2
#endif

/* ===== FILE structure ===== */
#define _F_READ     0x01
#define _F_WRITE    0x02
#define _F_EOF      0x04
#define _F_ERR      0x08
#define _F_OPEN     0x10

typedef struct _FILE {
    unsigned char fd;           /* DSS handle (0-9), 0xFF = console */
    unsigned char flags;
    unsigned char ungetc_buf;   /* 0xFF = empty */
} FILE;

/* File table and standard streams */
extern FILE _stdio_files[FOPEN_MAX];
extern FILE *stdin;
extern FILE *stdout;
extern FILE *stderr;

/* ===== File operations ===== */
FILE *fopen(const char *path, const char *mode);
int   fclose(FILE *fp);
size_t fread(void *buf, size_t size, size_t count, FILE *fp);
size_t fwrite(const void *buf, size_t size, size_t count, FILE *fp);
int   fseek(FILE *fp, long offset, int whence);
long  ftell(FILE *fp);
void  rewind(FILE *fp);
int   feof(FILE *fp);
int   ferror(FILE *fp);

/* ===== Character I/O ===== */
int   fgetc(FILE *fp);
int   fputc(int c, FILE *fp);
int   fputs(const char *s, FILE *fp);
char *fgets(char *buf, int size, FILE *fp);
int   ungetc(int c, FILE *fp);

/* ===== Console I/O ===== */
int   putchar(int c);
int   puts(const char *s);
int   getchar(void);

/* ===== Formatted I/O ===== */
int   printf(const char *fmt, ...);
int   sprintf(char *buf, const char *fmt, ...);
int   fprintf(FILE *fp, const char *fmt, ...);

/* ===== File management ===== */
int   remove(const char *path);
int   rename(const char *oldpath, const char *newpath);

#endif /* _STDIO_H */
