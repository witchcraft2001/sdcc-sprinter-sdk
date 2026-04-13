#ifndef _STDLIB_H
#define _STDLIB_H

#include <stddef.h>
#include <sprinter/dss.h>

#define EXIT_SUCCESS 0
#define EXIT_FAILURE 1
#define RAND_MAX 32767

int atoi(const char *s);
int abs(int n);
void exit(int status);
int rand(void);
void srand(unsigned int seed);

/* Environment — ANSI C / POSIX compatible */
char *getenv(const char *name);
#define putenv(s)   ((int)dss_setenv(s))

#endif /* _STDLIB_H */
