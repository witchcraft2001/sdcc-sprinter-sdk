# Перенос программ с SOLID C

Данная глава -- подробное руководство по портированию программ, написанных для компилятора SOLID C (Borland C 3.1 для ZX Sprinter), на SDCC Sprinter SDK.

## Общие изменения синтаксиса

### Объявления функций: K&R -> ANSI

SOLID C поддерживает устаревший синтаксис K&R для объявления параметров функций. SDCC требует стандартный ANSI-синтаксис:

```c
/* SOLID C (K&R-стиль) */
void func(a, b)
int a, b;
{
    /* ... */
}

/* SDCC SDK (ANSI-стиль) */
void func(int a, int b) {
    /* ... */
}
```

### Функция main

```c
/* SOLID C */
main()
{
    /* ... */
}

/* SDCC SDK */
void main(void) {
    /* ... */
}
```

SDCC предупредит, если `main` объявлена с возвращаемым типом `int`, так как на Z80 нет стандартного механизма возврата кода из `main` в ОС (CRT0 обрабатывает это самостоятельно).

### Прагмы #pragma nonrec / recursive -- удалить

SOLID C использует `#pragma nonrec` и `#pragma recursive` для управления размещением локальных переменных (в стеке или в статической памяти). В SDCC это не нужно -- компилятор сам определяет, может ли функция быть рекурсивной.

```c
/* SOLID C */
#pragma nonrec
void fast_func(int x) { /* ... */ }
#pragma recursive
void rec_func(int x) { /* ... */ }

/* SDCC SDK -- просто удалите прагмы */
void fast_func(int x) { /* ... */ }
void rec_func(int x) { /* ... */ }
```

### Функции с переменным числом параметров

SOLID C использует точку `.` вместо стандартного `...`:

```c
/* SOLID C */
int myprintf(fmt, .)
char *fmt;
{
    /* работа с аргументами через адресную арифметику */
}

/* SDCC SDK */
#include <stdarg.h>

int myprintf(const char *fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    /* работа с аргументами через va_arg */
    va_end(ap);
    return 0;
}
```

## Соответствие типов данных

| SOLID C | SDCC SDK | Размер |
|---------|----------|--------|
| `char` | `char` | 1 байт |
| `TINY` | `char` или `u8` | 1 байт |
| `BOOL` | `bool` или `u8` | 1 байт |
| `int` | `int` | 2 байта |
| `uint` | `unsigned int` или `u16` | 2 байта |
| `long` | `long` или `i32` | 4 байта |
| `f_point` | `long` или `u32` | 4 байта |
| `FD` | `int` или `i16` | 2 байта (файловый дескриптор) |
| `BYTE` | `u8` | 1 байт |
| `WORD` | `u16` | 2 байта |
| `DWORD` | `u32` | 4 байта |

> **Важно:** SOLID C поддерживает `float` (эмуляция). В SDCC Sprinter SDK нет поддержки чисел с плавающей точкой. Если программа использует `float`, нужно переработать логику на целочисленную арифметику или фиксированную точку.

## Соответствие заголовочных файлов

| SOLID C | SDCC SDK | Комментарий |
|---------|----------|-------------|
| `<stdio.h>` | `<stdio.h>` | Совместимо: printf, fopen, fclose, fread, fwrite, puts, getchar |
| `<dos.h>` | `<sprinter/dss.h>` | Функции переименованы: ffirst->dss_ffirst, fnext->dss_fnext |
| `<conio.h>` | `<conio.h>` | Совместимо: getch, kbhit, clrscr, gotoxy, cputs, cprintf |
| `<io.h>` | `<sprinter/dss.h>` | Функции переименованы: open->dss_open, close->dss_close |
| `<mouse.h>` | `<sprinter/mouse.h>` | API отличается (другие структуры и функции) |
| `<types.h>` | `<sprinter/types.h>` | Типы переименованы: BYTE->u8, WORD->u16, DWORD->u32 |
| `<string.h>` | `<string.h>` | Полностью совместимо |
| `<malloc.h>` | -- | **Недоступен.** Нет malloc/free |

## Соответствие функций

### Стандартный ввод-вывод (stdio.h)

Эти функции работают **без изменений**:

```c
printf(fmt, ...);
sprintf(buf, fmt, ...);
fprintf(fp, fmt, ...);
puts(s);
putchar(c);
getchar();
fopen(path, mode);
fclose(fp);
fread(buf, size, count, fp);
fwrite(buf, size, count, fp);
fgets(buf, size, fp);
fputs(s, fp);
fseek(fp, offset, whence);
remove(path);
```

### Консольный ввод-вывод (conio.h)

Эти функции работают **без изменений**:

```c
getch();
getche();
kbhit();
clrscr();
gotoxy(x, y);
cputs(s);
cprintf(fmt, ...);
putch(c);
```

### Низкоуровневый файловый ввод-вывод (io.h -> dss.h)

| SOLID C (io.h) | SDCC SDK (sprinter/dss.h) | Примечание |
|-----------------|---------------------------|------------|
| `open(path, mode)` | `dss_open(path, mode)` | Возвращает i16 |
| `close(fd)` | `dss_close(fd)` | fd -- u8 |
| `read(fd, buf, n)` | `dss_read(fd, buf, n)` | |
| `write(fd, buf, n)` | `dss_write(fd, buf, n)` | |
| `lseek(fd, off, whence)` | `dss_seek(fd, off, whence)` | off -- u32 |
| `creat(path)` | `dss_creat(path)` | |
| `unlink(path)` | `dss_delete(path)` | |

### Поиск файлов (dos.h -> dss.h)

| SOLID C (dos.h) | SDCC SDK (sprinter/dss.h) | Примечание |
|------------------|---------------------------|------------|
| `ffirst(pat, &find, attr)` | `dss_ffirst(pat, &find, attr)` | Другая структура! |
| `fnext(&find)` | `dss_fnext(&find)` | |

Структура результата поиска различается:

```c
/* SOLID C */
typedef struct {
    BYTE reserved[21];
    BYTE attr;
    WORD time;
    WORD date;
    long size;
    char name[13];
} FIND;

/* SDCC SDK */
typedef struct {
    char     name[8];
    char     ext[3];
    u8       attrib;
    u8       reserved[10];
    u16      time;
    u16      date;
    u16      cluster;
    u16      size_lo;
    u16      size_hi;
    u8       attr;
    char     ff_name[223];   /* Имя файла здесь */
} dss_find_t;
```

Ключевые отличия:
- В SOLID C имя файла: `find.name` (13 символов, формат 8.3)
- В SDK имя файла: `find.ff_name` (длинное имя, null-terminated)
- В SOLID C размер: `find.size` (long)
- В SDK размер: `find.size_lo` + `find.size_hi` (два u16)

### Системные вызовы

| SOLID C | SDCC SDK | Примечание |
|---------|----------|------------|
| `bdos(func, ...)` | Используйте `dss_*` функции напрямую | |
| `intdos(func, ...)` | Используйте `dss_*` функции напрямую | |
| `inp(port)` | `inp(port)` | Совместимо (через bios.h) |
| `outp(port, val)` | `outp(port, val)` | Совместимо (через bios.h) |
| `exit(code)` | `exit(code)` | Совместимо (через stdlib.h) |

### Мышь (mouse.h)

API мыши отличается значительно:

| SOLID C | SDCC SDK | Примечание |
|---------|----------|------------|
| `ms_init()` | `mouse_init()` | Возвращает 1/0 |
| `ms_show()` | `mouse_show()` | |
| `ms_hide()` | `mouse_hide()` | |
| `ms_stat(&x, &y, &btn)` | `mouse_stat(&state)` | Одна структура вместо трёх параметров |
| `ms_setpos(x, y)` | `mouse_setpos(x, y)` | |
| `ms_xbound(min, max)` | `mouse_xbound(min, max)` | |
| `ms_ybound(min, max)` | `mouse_ybound(min, max)` | |

В SOLID C `ms_stat` возвращает координаты и кнопки через отдельные указатели. В SDK используется структура `mouse_state_t`:

```c
/* SOLID C */
int x, y, btn;
ms_stat(&x, &y, &btn);

/* SDCC SDK */
mouse_state_t ms;
mouse_stat(&ms);
/* ms.x, ms.y, ms.buttons */
```

## Пошаговый пример: портирование SORT2.C

Рассмотрим портирование реальной программы -- алгоритмов сортировки из SOLID C.

### Оригинал SOLID C (фрагмент)

```c
/* SORT2.C for SOLID C */
#include <stdio.h>
#include <conio.h>

#define SIZE 15

uint iterations, exchanges, conditions;

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

main()
{
    int arr[SIZE];
    /* ... */
    getch();
}
```

### Портированная версия для SDCC SDK

```c
/* sort.c for SDCC Sprinter SDK */
#include <stdio.h>
#include <stdlib.h>
#include <conio.h>

#define SIZE 15

/* 1. uint -> unsigned int */
unsigned int iterations;
unsigned int exchanges;
unsigned int conditions;

/* 2. K&R -> ANSI объявления параметров */
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

/* 4. main() -> void main(void) */
void main(void) {
    int arr[SIZE];

    printf("=== Sorting Algorithms Demo ===\n");
    printf("Array size: %d elements\n", SIZE);

    /* 5. Добавлен #include <stdlib.h> для srand/rand */
    srand(12345);

    /* Заполнение массива */
    {
        int i;
        for (i = 0; i < SIZE; i++)
            arr[i] = rand() % 100 - 50;
    }

    iterations = exchanges = conditions = 0;
    bubble_sort(arr, SIZE);

    printf("After:  ");
    print_array(arr, SIZE);
    printf("  Iterations: %u  Exchanges: %u\n",
           iterations, exchanges);

    printf("\nDone! Press any key.\n");
    getch();
}
```

### Сводка изменений

| # | Что изменено | Было | Стало |
|---|-------------|------|-------|
| 1 | Тип `uint` | `uint iterations;` | `unsigned int iterations;` |
| 2 | Параметры функций | K&R: `func(a,b) int a,b;` | ANSI: `func(int a, int b)` |
| 3 | Прагмы | `#pragma nonrec` | Удалить |
| 4 | Функция main | `main()` | `void main(void)` |
| 5 | stdlib.h | Не нужен в SOLID | Добавить для rand/srand |

## Чек-лист портирования

1. [ ] Заменить объявления K&R на ANSI
2. [ ] Изменить `main()` на `void main(void)`
3. [ ] Удалить `#pragma nonrec` / `#pragma recursive`
4. [ ] Заменить `.` на `...` в вариативных функциях, использовать `<stdarg.h>`
5. [ ] Заменить типы: `uint`->`unsigned int`, `TINY`/`BOOL`->`u8`, `FD`->`i16`, `f_point`->`u32`
6. [ ] Заменить `<dos.h>` на `<sprinter/dss.h>`, переименовать функции
7. [ ] Заменить `<io.h>` на `<sprinter/dss.h>`, переименовать функции
8. [ ] Заменить `<mouse.h>` на `<sprinter/mouse.h>`, адаптировать API
9. [ ] Заменить `<types.h>` на `<sprinter/types.h>`, переименовать типы
10. [ ] Убрать `<malloc.h>` -- динамическая память недоступна
11. [ ] Убрать `float` -- переработать на целочисленную арифметику
12. [ ] Заменить `FIND` на `dss_find_t`, адаптировать поля структуры
13. [ ] Проверить, что `\r\n` используется для `dss_puts`, а `\n` для `printf`
14. [ ] Собрать (`make`) и проверить предупреждения компилятора
15. [ ] Протестировать на эмуляторе или реальном железе
