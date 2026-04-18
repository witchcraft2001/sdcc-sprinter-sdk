# API Спринтера

Для прямого доступа к аппаратуре ZX Sprinter SDK предоставляет набор специализированных заголовков. Они независимы от стандартной библиотеки и вызывают системные функции DSS/BIOS напрямую.

Общий заголовок `<sprinter.h>` подключает все API-заголовки разом.

## DSS API -- sprinter/dss.h

DSS (Disk SubSystem) -- операционная система ZX Sprinter. Все вызовы выполняются через вектор `RST #10`, номер функции передаётся в регистре C.

### Консольный вывод

```c
void dss_puts(const char *str);    /* Вывод строки */
void dss_putchar(u8 ch);           /* Вывод символа */
void dss_clrscr(void);             /* Очистка экрана */
void dss_gotoxy(u8 x, u8 y);      /* Установка курсора (1-based) */
```

> **Примечание:** `dss_puts` ожидает `\r\n` для перевода строки (не `\n`).

### Клавиатура

```c
u8   dss_waitkey(void);     /* Ожидание нажатия клавиши */
bool dss_kbhit(void);       /* Есть ли символ в буфере? */
u16  dss_getche(void);      /* Чтение клавиши с эхом */
```

### Файловые операции

```c
i16  dss_open(const char *path, u8 mode);    /* Открыть файл */
i16  dss_creat(const char *path);            /* Создать файл */
u8   dss_close(u8 fd);                       /* Закрыть файл */
i16  dss_read(u8 fd, void *buf, u16 count);  /* Чтение */
i16  dss_write(u8 fd, const void *buf, u16 count);  /* Запись */
i16  dss_seek(u8 fd, u32 offset, u8 origin); /* Позиционирование */
u8   dss_delete(const char *path);           /* Удаление файла */
u8   dss_rename(const char *old, const char *new);  /* Переименование */
```

Режимы открытия (`mode`):

| Константа | Значение | Описание |
|-----------|----------|----------|
| `O_RDONLY` | 0x00 | Только чтение |
| `O_WRONLY` | 0x01 | Только запись |
| `O_RDWR` | 0x02 | Чтение и запись |
| `O_CREAT` | 0x04 | Создать, если не существует |
| `O_TRUNC` | 0x08 | Обрезать при открытии |
| `O_APPEND` | 0x10 | Дописывать в конец |

Пример низкоуровневого файлового ввода-вывода:

```c
#include <sprinter/dss.h>

void main(void) {
    i16 fd;
    char buf[64];
    i16 n;

    /* Запись */
    fd = dss_creat("TEST.TXT");
    if (fd >= 0) {
        dss_write((u8)fd, "Hello!\r\n", 8);
        dss_close((u8)fd);
    }

    /* Чтение */
    fd = dss_open("TEST.TXT", O_RDONLY);
    if (fd >= 0) {
        n = dss_read((u8)fd, buf, sizeof(buf) - 1);
        if (n > 0) {
            buf[n] = 0;
            dss_puts(buf);
        }
        dss_close((u8)fd);
    }

    dss_delete("TEST.TXT");
    dss_waitkey();
}
```

### Каталоги

```c
u8   dss_chdir(const char *path);
i8   dss_ffirst(const char *pattern, dss_find_t *result, u8 attr);
i8   dss_fnext(dss_find_t *result);
```

Структура `dss_find_t` содержит результат поиска:

```c
typedef struct {
    char     name[8];       /* Шаблон имени */
    char     ext[3];        /* Шаблон расширения */
    u8       attrib;        /* Атрибут поиска */
    u8       reserved[10];
    u16      time;          /* Время файла */
    u16      date;          /* Дата файла */
    u16      cluster;       /* Первый кластер */
    u16      size_lo;       /* Размер (младшее слово) */
    u16      size_hi;       /* Размер (старшее слово) */
    u8       attr;          /* Атрибут найденного файла */
    char     ff_name[223];  /* Имя файла (null-terminated) */
} dss_find_t;
```

Пример листинга каталога:

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
    getchar();
}
```

### Управление памятью

```c
void dss_setwin(u8 win, u8 page);   /* Подключить страницу к окну */
u8   dss_getmem(void);              /* Выделить страницу (или 0xFF) */
void dss_freemem(u8 page);          /* Освободить страницу */
```

### Дата и время

```c
void dss_getdate(dss_date_t *d);
void dss_gettime(dss_time_t *t);
```

```c
typedef struct {
    u16  year;
    u8   day;
    u8   month;
} dss_date_t;

typedef struct {
    u8   minute;
    u8   hour;
    u8   hundredths;
    u8   second;
} dss_time_t;
```

### Системные функции

```c
void dss_exit(u8 code);            /* Завершение программы */
i16  dss_exec(const char *path);   /* Запуск другой программы */
i16  dss_exec_ex(const char *path, u8 *err);  /* Запуск с возвратом кода ошибки DSS */
void dss_ei(void);                 /* Разрешить прерывания */
void dss_di(void);                 /* Запретить прерывания */
u8   dss_getdisk(void);            /* Получить текущий диск */
void dss_setdisk(u8 disk);         /* Установить текущий диск */
char *dss_cmdline(void);           /* Указатель на командную строку */
```

`dss_exec()` сохраняет совместимость со старым кодом: при ошибке возвращает только `-1`.

Если нужен точный код ошибки DSS (то, что DSS возвращает в регистре `A`), используйте `dss_exec_ex()`:

```c
#include <stdio.h>
#include <sprinter.h>

void main(void) {
    u8 err;
    i16 rc = dss_exec_ex("CHILD.EXE arg1", &err);

    if (rc < 0) {
        printf("EXEC error: A=0x%X (%u)\n", (u16)err, (u16)err);
    } else {
        printf("Код завершения дочерней программы: %d\n", rc);
    }
}
```

## BIOS API -- sprinter/bios.h

BIOS предоставляет низкоуровневый доступ к аппаратуре через вектор `RST #08`. Для обычного программирования рекомендуются вызовы DSS, но BIOS нужен для графики, палитры и прямой работы с портами.

### Палитра

```c
void bios_setpal(u8 index, u8 r, u8 g, u8 b);
```

Устанавливает цвет палитры:
- `index`: номер цвета (0-255)
- `r`, `g`, `b`: компоненты цвета (0-63 каждый, 6-битные)

### Пиксели

```c
void bios_putpixel(u16 x, u8 y, u8 color);
```

Рисует точку в графическом режиме:
- `x`: 0-319 (режим 320x256) или 0-639 (режим 640x256)
- `y`: 0-255
- `color`: индекс цвета в палитре

### Версия и идентификация

```c
u16 bios_version(void);     /* Версия BIOS (BCD) */
u8  bios_board_id(void);    /* Идентификатор платы */
```

### Порты ввода-вывода

```c
u8   inp(u16 port);              /* Чтение порта */
void outp(u16 port, u8 value);   /* Запись в порт */
```

## Video API -- sprinter/video.h

### Видеорежимы

```c
void video_setmode(u8 mode);
u8   video_getmode(void);
```

Доступные режимы:

| Константа | Значение | Описание |
|-----------|----------|----------|
| `VMODE_TEXT40` | 0x02 | Текстовый 40x32 |
| `VMODE_TEXT80` | 0x03 | Текстовый 80x32 |
| `VMODE_ZX` | 0x03 | Текстовый (синоним TEXT80) |
| `VMODE_320_16` | 0x80 | 320x256, 16 цветов |
| `VMODE_320` | 0x81 | 320x256, 256 цветов (8 бит/пиксель) |
| `VMODE_640_16` | 0x82 | 640x256, 16 цветов |

### Двойная буферизация

```c
void video_swap(void);      /* Переключить буфер */
void video_vsync(void);     /* Ожидание вертикальной синхронизации */
```

### Палитра (через видео API)

```c
void video_setpal(u8 index, u8 r, u8 g, u8 b);
```

В отличие от `bios_setpal`, здесь `r`, `g`, `b` принимают значения 0-255.

### Работа с VRAM

```c
void video_mapvram(u8 win, u8 page);  /* Подключить VRAM-страницу к окну памяти */
```

Параметры:
- `win`: окно памяти (0-3)
- `page`: номер VRAM-страницы (0x50-0x5F)

### Безопасный PORT_Y

```c
void video_safe_porty(void);
```

**Обязательно** вызывайте эту функцию после завершения рисования пикселей! Функция `bios_putpixel` использует регистр PORT_Y для выбора строки VRAM. Если не сбросить PORT_Y, последующие обращения к памяти (код, данные, стек) могут повредить VRAM.

## Mouse API -- sprinter/mouse.h

Драйвер мыши работает через вектор `RST #30`.

### Инициализация и курсор

```c
u8   mouse_init(void);       /* Инициализация (1=мышь есть, 0=нет) */
void mouse_show(void);       /* Показать курсор */
void mouse_hide(void);       /* Скрыть курсор */
```

### Состояние мыши

```c
void mouse_stat(mouse_state_t *state);
```

Структура состояния:

```c
typedef struct {
    u8   buttons;    /* Кнопки (MS_BTN_LEFT, MS_BTN_RIGHT, MS_BTN_MIDDLE) */
    u16  x;          /* Координата X */
    u16  y;          /* Координата Y */
} mouse_state_t;
```

Маски кнопок:
- `MS_BTN_LEFT` (0x01) -- левая кнопка
- `MS_BTN_RIGHT` (0x02) -- правая кнопка
- `MS_BTN_MIDDLE` (0x04) -- средняя кнопка

### Позиционирование

```c
void mouse_setpos(u16 x, u16 y);
void mouse_xbound(u16 min_x, u16 max_x);
void mouse_ybound(u16 min_y, u16 max_y);
```

Пример:

```c
#include <sprinter.h>

void main(void) {
    mouse_state_t ms;

    if (!mouse_init()) {
        dss_puts("Mouse not found!\r\n");
        dss_waitkey();
        return;
    }

    mouse_xbound(0, 319);
    mouse_ybound(0, 255);
    mouse_show();

    while (1) {
        mouse_stat(&ms);
        if (ms.buttons & MS_BTN_RIGHT)
            break;
    }

    mouse_hide();
}
```

## Порты -- sprinter/ports.h

Определения адресов аппаратных портов:

### Окна памяти

| Константа | Адрес | Описание |
|-----------|-------|----------|
| `PORT_WIN0` | 0x82 | Окно 0: 0x0000-0x3FFF |
| `PORT_WIN1` | 0xA2 | Окно 1: 0x4000-0x7FFF |
| `PORT_WIN2` | 0xC2 | Окно 2: 0x8000-0xBFFF |
| `PORT_WIN3` | 0xE2 | Окно 3: 0xC000-0xFFFF |

### Видео

| Константа | Адрес | Описание |
|-----------|-------|----------|
| `PORT_GFXMODE` | 0xC3 | Графический режим |
| `PORT_RGMOD` | 0xC9 | Переключение буферов, экран вкл/выкл |

### Звук

| Константа | Адрес | Описание |
|-----------|-------|----------|
| `PORT_AY_ADDR` | 0x8C | Адрес регистра AY-3-8910 |
| `PORT_AY_WRITE` | 0x8D | Запись данных AY |
| `PORT_AY_READ` | 0x8E | Чтение данных AY |
| `PORT_COVOX` | 0x88 | Вывод Covox DAC |

### Клавиатура и мышь

| Константа | Адрес | Описание |
|-----------|-------|----------|
| `PORT_KEYB` | 0xFE | Клавиатура + цвет бордюра |
| `PORT_KEMPSTON` | 0x1F | Джойстик Kempston |
| `PORT_MOUSE_BTN` | 0x58 | Кнопки мыши |

## Типы -- sprinter/types.h

Фиксированные целочисленные типы:

| Тип | Размер | Диапазон |
|-----|--------|----------|
| `u8` / `uint8_t` | 1 байт | 0..255 |
| `i8` / `int8_t` | 1 байт | -128..127 |
| `u16` / `uint16_t` | 2 байта | 0..65535 |
| `i16` / `int16_t` | 2 байта | -32768..32767 |
| `u32` / `uint32_t` | 4 байта | 0..4294967295 |
| `i32` / `int32_t` | 4 байта | -2147483648..2147483647 |

Также определены классические типы:
- `byte` = `unsigned char`
- `word` = `unsigned int`
- `bool`, `true`, `false` (через `<stdbool.h>`)
