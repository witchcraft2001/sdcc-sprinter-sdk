/**
 * conio.h — Console I/O for ZX Sprinter SDK
 *
 * Wraps DSS console functions with familiar conio-style API.
 */

#ifndef _CONIO_H
#define _CONIO_H

#include <stddef.h>

void clrscr(void);
void gotoxy(int x, int y);
int  getch(void);
int  getche(void);
int  kbhit(void);
void cputs(const char *s);
int  cprintf(const char *fmt, ...);
int  putch(int c);

#endif /* _CONIO_H */
