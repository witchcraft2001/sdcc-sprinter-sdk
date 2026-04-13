/* _stdio_init.c — FILE table and stdin/stdout/stderr */
#include <stdio.h>

FILE _stdio_files[FOPEN_MAX] = {
    { 0xFF, _F_READ  | _F_OPEN, 0xFF },  /* stdin  */
    { 0xFF, _F_WRITE | _F_OPEN, 0xFF },  /* stdout */
    { 0xFF, _F_WRITE | _F_OPEN, 0xFF },  /* stderr */
    /* remaining 7 entries zero-initialized (closed) */
};

FILE *stdin  = &_stdio_files[0];
FILE *stdout = &_stdio_files[1];
FILE *stderr = &_stdio_files[2];
