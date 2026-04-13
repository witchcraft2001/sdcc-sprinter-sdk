# Стандартная библиотека C

## Обзор

SDK предоставляет подмножество стандартной библиотеки языка Си, достаточное для большинства задач на ZX Sprinter:

| Заголовок | Назначение | Модулей |
|-----------|------------|---------|
| `<stdio.h>` | Ввод-вывод (printf, файлы, потоки) | 17 |
| `<stdlib.h>` | Утилиты (exit, rand, atoi) | 4 |
| `<string.h>` | Работа со строками и памятью | 12 |
| `<ctype.h>` | Классификация символов | 12 |
| `<conio.h>` | Консольный ввод-вывод | 8 |

Каждая функция скомпилирована в отдельный модуль. Линковщик включает только используемые функции, что критически важно для экономии памяти на Z80.

> **Важные ограничения:**
> - Нет `malloc` / `free` (динамическое выделение памяти недоступно)
> - Нет чисел с плавающей точкой (`float`, `double`)
> - Целые числа `int` -- 16 бит (диапазон -32768..32767)
> - `long` -- 32 бита

## stdio.h -- Стандартный ввод-вывод

### Структура FILE

Файлы представлены структурой `FILE`:

```c
typedef struct _FILE {
    unsigned char fd;           /* DSS-дескриптор (0-9), 0xFF = консоль */
    unsigned char flags;        /* Флаги: чтение, запись, EOF, ошибка */
    unsigned char ungetc_buf;   /* Буфер ungetc (0xFF = пуст) */
} FILE;
```

Максимум одновременно открытых файлов: **10** (`FOPEN_MAX`).

### Стандартные потоки

```c
extern FILE *stdin;    /* Ввод с консоли */
extern FILE *stdout;   /* Вывод на консоль */
extern FILE *stderr;   /* Вывод ошибок (= консоль) */
```

Все три потока связаны с консолью DSS. Их не нужно открывать или закрывать.

### Работа с файлами

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

Режимы `fopen`:

| Режим | Описание |
|-------|----------|
| `"r"` | Чтение (файл должен существовать) |
| `"w"` | Запись (создаёт новый или обрезает существующий) |
| `"a"` | Дополнение (создаёт или дописывает в конец) |
| `"r+"` | Чтение и запись (файл должен существовать) |
| `"w+"` | Чтение и запись (создаёт или обрезает) |

Пример:

```c
#include <stdio.h>

void main(void) {
    FILE *fp;
    char buf[64];

    /* Запись в файл */
    fp = fopen("TEST.TXT", "w");
    if (fp) {
        fputs("Hello, Sprinter!\n", fp);
        fclose(fp);
    }

    /* Чтение из файла */
    fp = fopen("TEST.TXT", "r");
    if (fp) {
        while (fgets(buf, sizeof(buf), fp)) {
            printf("%s", buf);
        }
        fclose(fp);
    }
}
```

### Посимвольный ввод-вывод

```c
int   fgetc(FILE *fp);
int   fputc(int c, FILE *fp);
int   fputs(const char *s, FILE *fp);
char *fgets(char *buf, int size, FILE *fp);
int   ungetc(int c, FILE *fp);
```

### Консольный ввод-вывод

```c
int   putchar(int c);      /* Вывод символа */
int   puts(const char *s);  /* Вывод строки + \n */
int   getchar(void);        /* Чтение символа */
```

### Форматированный вывод

```c
int   printf(const char *fmt, ...);
int   sprintf(char *buf, const char *fmt, ...);
int   fprintf(FILE *fp, const char *fmt, ...);
```

#### Поддерживаемые спецификаторы printf

| Спецификатор | Описание | Пример |
|--------------|----------|--------|
| `%d`, `%i` | Знаковое целое (десятичное) | `printf("%d", -42)` -> `-42` |
| `%u` | Беззнаковое целое | `printf("%u", 65535)` -> `65535` |
| `%x` | Шестнадцатеричное (строчные) | `printf("%x", 255)` -> `ff` |
| `%X` | Шестнадцатеричное (заглавные) | `printf("%X", 255)` -> `FF` |
| `%s` | Строка | `printf("%s", "hi")` -> `hi` |
| `%c` | Символ | `printf("%c", 65)` -> `A` |
| `%%` | Литерал `%` | `printf("100%%")` -> `100%` |

#### Модификаторы ширины

```c
printf("[%5d]",  42);     /* [   42] -- правое выравнивание */
printf("[%-5d]", 42);     /* [42   ] -- левое выравнивание */
printf("[%05d]", 42);     /* [00042] -- заполнение нулями */
printf("[%04X]", 255);    /* [00FF]  -- hex с нулями */
```

#### Ограничения printf

- **Нет float/double:** спецификаторы `%f`, `%e`, `%g` не поддерживаются
- **Модификатор `%l`:** принимается, но целочисленные преобразования работают как 16-битные. `%ld` работает как `%d`
- Нет `%o` (восьмеричное)
- Нет `%p` (указатель)

## stdlib.h -- Утилиты

```c
void  exit(int code);           /* Завершение программы */
int   atoi(const char *s);      /* Строка -> целое число */
int   abs(int n);                /* Абсолютное значение */
int   rand(void);                /* Псевдослучайное число (0..32767) */
void  srand(unsigned int seed);  /* Инициализация генератора */
```

Пример:

```c
#include <stdio.h>
#include <stdlib.h>

void main(void) {
    int i;
    srand(12345);
    for (i = 0; i < 10; i++) {
        printf("rand() = %d\n", rand() % 100);
    }
    printf("atoi(\"42\") = %d\n", atoi("42"));
    printf("abs(-7) = %d\n", abs(-7));
}
```

## string.h -- Работа со строками

SDK предоставляет 12 функций для работы со строками и памятью:

### Длина и копирование

```c
size_t strlen(const char *s);
char  *strcpy(char *dst, const char *src);
char  *strncpy(char *dst, const char *src, size_t n);
```

### Сравнение

```c
int    strcmp(const char *s1, const char *s2);
int    strncmp(const char *s1, const char *s2, size_t n);
```

### Конкатенация

```c
char  *strcat(char *dst, const char *src);
char  *strncat(char *dst, const char *src, size_t n);
```

### Поиск

```c
char  *strchr(const char *s, int c);
char  *strrchr(const char *s, int c);
```

### Операции с памятью

```c
void  *memcpy(void *dst, const void *src, size_t n);
void  *memset(void *dst, int c, size_t n);
int    memcmp(const void *s1, const void *s2, size_t n);
```

Пример:

```c
#include <stdio.h>
#include <string.h>

void main(void) {
    char buf[80];
    strcpy(buf, "Hello");
    strcat(buf, ", ");
    strcat(buf, "Sprinter!");
    printf("%s (len=%d)\n", buf, strlen(buf));
    /* Hello, Sprinter! (len=17) */
}
```

## ctype.h -- Классификация символов

12 функций для проверки и преобразования символов:

### Проверка типа символа

| Функция | Описание |
|---------|----------|
| `isalpha(c)` | Буква (A-Z, a-z) |
| `isdigit(c)` | Цифра (0-9) |
| `isalnum(c)` | Буква или цифра |
| `isspace(c)` | Пробельный символ (пробел, табуляция, \n, \r) |
| `isupper(c)` | Заглавная буква |
| `islower(c)` | Строчная буква |
| `isprint(c)` | Печатный символ |
| `isxdigit(c)` | Шестнадцатеричная цифра (0-9, A-F, a-f) |
| `iscntrl(c)` | Управляющий символ |
| `ispunct(c)` | Знак пунктуации |

### Преобразование регистра

```c
int toupper(int c);    /* a-z -> A-Z */
int tolower(int c);    /* A-Z -> a-z */
```

## conio.h -- Консольный ввод-вывод

API в стиле Turbo C / CP/M для прямой работы с консолью:

```c
int   getch(void);              /* Чтение клавиши (без эха) */
int   getche(void);             /* Чтение клавиши (с эхом) */
int   kbhit(void);              /* Проверка: нажата ли клавиша? */
int   putch(int c);             /* Вывод символа */
void  cputs(const char *s);     /* Вывод строки */
int   cprintf(const char *fmt, ...);  /* Форматированный вывод */
void  clrscr(void);             /* Очистка экрана */
void  gotoxy(int x, int y);     /* Установка курсора (1-based) */
```

Пример:

```c
#include <conio.h>

void main(void) {
    int ch;
    clrscr();
    gotoxy(1, 1);
    cputs("Press keys (ESC to quit):\n");
    do {
        ch = getch();
        cprintf("Key code: %d\n", ch);
    } while (ch != 27);
}
```

> **Примечание:** `gotoxy` использует координаты, начинающиеся с 1 (как в Turbo C), а не с 0.
