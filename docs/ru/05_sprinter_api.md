# API Спринтера

ZX Sprinter SDK даёт прямой доступ к вызовам DSS, BIOS, видео, мыши и аппаратным портам. Эти заголовки независимы от стандартной библиотеки и полезны там, где нужен меньший размер бинарника или более прямой контроль над железом.

Подключить всё сразу:

```c
#include <sprinter.h>
```

Или только нужные части:

```c
#include <sprinter/dss.h>
#include <sprinter/bios.h>
#include <sprinter/video.h>
#include <sprinter/mouse.h>
#include <sprinter/assets.h>
#include <sprinter/ay.h>
#include <sprinter/ports.h>
#include <sprinter/types.h>
```

## sprinter/types.h -- Базовые типы

Фиксированные целочисленные типы для Z80/SDCC:

| Тип | Размер | Диапазон |
|-----|--------|----------|
| `u8` / `uint8_t` | 1 байт | 0 .. 255 |
| `i8` / `int8_t` | 1 байт | -128 .. 127 |
| `u16` / `uint16_t` | 2 байта | 0 .. 65535 |
| `i16` / `int16_t` | 2 байта | -32768 .. 32767 |
| `u32` / `uint32_t` | 4 байта | 0 .. 4294967295 |
| `i32` / `int32_t` | 4 байта | -2147483648 .. 2147483647 |
| `byte` | 1 байт | Синоним `unsigned char` |
| `word` | 2 байта | Синоним `unsigned int` |
| `bool` | 1 байт | `true` или `false` |

Также через `<stddef.h>` доступны `size_t`, `NULL` и `offsetof`.

## sprinter/dss.h -- Вызовы DSS

Все системные вызовы DSS идут через `RST #10`. Обёртки в SDK согласованы с ABI SDCC 2.9.0.

### Консоль и клавиатура

```c
void dss_putchar(u8 ch);
void dss_puts(const char *str);
u8   dss_waitkey(void);
void dss_waitkey_ex(dss_key_t *key);
bool dss_scankey(dss_key_t *key);
bool dss_testkey(dss_key_t *key);
bool dss_kbhit(void);
u16  dss_getche(void);
void dss_gotoxy(u8 x, u8 y);
void dss_clrscr(void);
```

- `dss_putchar(ch)` выводит один символ.
- `dss_puts(str)` печатает zero-terminated строку как есть. Для новой строки используйте `\r\n`, а не просто `\n`.
- `dss_waitkey()` ждёт нажатия клавиши и возвращает ASCII-код.
- `dss_waitkey_ex(key)` ждёт клавишу и заполняет структуру с полным состоянием.
- `dss_scankey(key)` не блокирует выполнение: возвращает `true`, если символ был в буфере, и извлекает его.
- `dss_testkey(key)` тоже не блокирует, но символ из буфера не удаляет.
- `dss_kbhit()` быстро проверяет, есть ли что-то в клавиатурном буфере.
- `dss_getche()` читает клавишу с эхо на экран.
- `dss_gotoxy(x, y)` ставит текстовый курсор. Координаты 1-based.
- `dss_clrscr()` очищает активный текстовый экран.

Структура `dss_key_t`:

```c
typedef struct {
    u8 ascii;
    u8 scan;
    u8 modifiers;
    u8 locks;
} dss_key_t;
```

- `ascii` — ASCII-код, если он есть.
- `scan` — скан-код или позиционный код клавиши.
- `modifiers` — Shift/Ctrl/Alt.
- `locks` — Caps/Num/Scroll/Insert/RusLat.

Часто используемые маски:

```c
DSS_KEYMOD_ALT
DSS_KEYMOD_CTRL
DSS_KEYMOD_LSHIFT
DSS_KEYMOD_RSHIFT
DSS_KEYLOCK_CAPS
DSS_KEYLOCK_NUM
DSS_KEYLOCK_SCROLL
DSS_KEYLOCK_RUSLAT
```

### Файловый ввод-вывод

```c
i16  dss_open(const char *path, u8 mode);
i16  dss_creat(const char *path);
u8   dss_close(u8 fd);
i16  dss_read(u8 fd, void *buf, u16 count);
i16  dss_write(u8 fd, const void *buf, u16 count);
i16  dss_seek(u8 fd, u32 offset, u8 origin);
u8   dss_delete(const char *path);
u8   dss_rename(const char *oldpath, const char *newpath);
```

- `path` — путь DSS, например `"A:\\DIR\\FILE.TXT"` или `"FILE.TXT"`.
- `fd` — 8-битный файловый дескриптор DSS, который вернул `dss_open()` или `dss_creat()`.
- `buf` — буфер для чтения или записи.
- `count` — число байт для передачи.
- `offset` — 32-битное смещение в файле.
- `origin` — `SEEK_SET`, `SEEK_CUR` или `SEEK_END`.

Возвращаемые значения:

- `dss_open()` / `dss_creat()` возвращают дескриптор или `-1`.
- `dss_read()` / `dss_write()` возвращают число реально переданных байт или `-1`.
- `dss_seek()` возвращает `0` при успехе или `-1` при ошибке.
- `dss_delete()` / `dss_rename()` возвращают `0` при успехе, иначе код ошибки DSS.

Биты режима открытия можно комбинировать через OR:

| Константа | Значение | Описание |
|-----------|----------|----------|
| `O_RDONLY` | 0x00 | Только чтение |
| `O_WRONLY` | 0x01 | Только запись |
| `O_RDWR` | 0x02 | Чтение и запись |
| `O_CREAT` | 0x04 | Создать, если файла нет |
| `O_TRUNC` | 0x08 | Обрезать файл до нуля |
| `O_APPEND` | 0x10 | Всегда писать в конец |

### Каталоги и поиск файлов

```c
u8   dss_chdir(const char *path);
u8   dss_curdir(char *buf);
u8   dss_mkdir(const char *path);
u8   dss_rmdir(const char *path);
i8   dss_ffirst(const char *pattern, dss_find_t *result, u8 attr);
i8   dss_fnext(dss_find_t *result);
```

- `dss_chdir(path)` меняет текущий каталог.
- `dss_curdir(buf)` записывает текущий каталог в `buf`. Диск при этом не включается в строку.
- `dss_mkdir(path)` создаёт один уровень каталога.
- `dss_rmdir(path)` удаляет пустой каталог.
- `dss_ffirst(pattern, result, attr)` начинает поиск по маске вроде `"*.C"` или `"*.*"`.
- `dss_fnext(result)` продолжает уже начатый поиск, используя состояние в `result`.

Структура `dss_find_t` содержит и контекст поиска, и информацию о найденном файле:

```c
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
    char     ff_name[223];
} dss_find_t;
```

- `ff_name` — найденное имя файла, zero-terminated.
- `attr` — байт атрибутов найденного файла.
- `size_lo` и `size_hi` образуют 32-битный размер.
- `date` и `time` хранятся в DOS-совместимом packed-формате.

Биты атрибутов поиска:

| Константа | Значение | Описание |
|-----------|----------|----------|
| `FA_NORMAL` | 0x00 | Обычные файлы |
| `FA_RDONLY` | 0x01 | Только чтение |
| `FA_HIDDEN` | 0x02 | Скрытый |
| `FA_SYSTEM` | 0x04 | Системный |
| `FA_LABEL` | 0x08 | Метка тома |
| `FA_DIREC` | 0x10 | Каталог |
| `FA_ARCH` | 0x20 | Архивный |

### Дата и время

```c
void dss_getdate(dss_date_t *d);
void dss_gettime(dss_time_t *t);
void dss_settime(dss_date_t *d, dss_time_t *t);
```

- `dss_getdate(d)` заполняет `year`, `month`, `day`.
- `dss_gettime(t)` заполняет `hour`, `minute`, `second`, `hundredths`.
- `dss_settime(d, t)` меняет дату и время одним вызовом DSS. Оба указателя должны быть валидны.

Структуры:

```c
typedef struct {
    u16 year;
    u8  day;
    u8  month;
} dss_date_t;

typedef struct {
    u8 minute;
    u8 hour;
    u8 hundredths;
    u8 second;
} dss_time_t;
```

### Память по страницам

```c
void dss_setwin(u8 win, u8 page);
u8   dss_getmem(void);
void dss_freemem(u8 page);
void dss_meminfo(u16 *total, u16 *free_pages);
```

- `dss_setwin(win, page)` подключает RAM-страницу к пользовательскому окну DSS `1..3` (`WIN1`..`WIN3`).
- `dss_getmem()` выделяет одну страницу RAM и возвращает номер страницы (block id). Если DSS GETMEM сообщает об ошибке (`CF=1`, `A=код ошибки`), обёртка SDK возвращает `0xFF`; вызывающий код должен проверить это значение перед передачей в `dss_setwin()` или `dss_freemem()`.
- `dss_freemem(page)` возвращает страницу обратно в DSS.
- `dss_meminfo(total, free_pages)` сообщает общее число управляемых страниц и количество свободных.

### Процессы, пути, окружение и текстовые helper-вызовы DSS

```c
u16  dss_version(void);
void dss_exit(u8 code);
i16  dss_exec(const char *path);
i16  dss_exec_ex(const char *path, u8 *err);
u8   dss_wait(void);
char *dss_cmdline(void);
u16  dss_call(u16 addr);
u16  dss_callp(u16 addr, u16 param);
i8   dss_expath(const char *path, char *buf, u8 subfunc);
i8   dss_appinfo(u8 subfunc, char *buf);
i8   dss_getenv(const char *name, char *buf);
i8   dss_setenv(const char *namevalue);
u8   dss_getdisk(void);
void dss_setdisk(u8 disk);
void dss_ei(void);
void dss_di(void);
u8   dss_setvmod(u8 mode, u8 page);
void dss_getvmod(u8 *mode, u8 *page);
void dss_scroll(u8 x, u8 y, u8 w, u8 h, u8 dir, u8 count);
void dss_clear(u8 x, u8 y, u8 w, u8 h, u8 color, u8 attr);
```

- `dss_version()` возвращает версию DSS в packed-виде `(major << 8) | minor`.
- `dss_exit(code)` завершает текущую программу и отдаёт код возврата в DSS.
- `dss_exec(path)` запускает другую программу и возвращает её exit code, либо `-1`, если запуск не удался.
- `dss_exec_ex(path, err)` делает то же, но при ошибке запуска дополнительно пишет raw-код DSS в `*err`.
- `dss_wait()` возвращает `ERLEVEL` DSS, то есть последний код завершения дочерней программы.
- `dss_cmdline()` возвращает указатель на сохранённый command tail текущего процесса. Если он нужен надолго, лучше скопировать строку.
- `dss_call(addr)` вызывает машинный код по адресу `addr` и возвращает то, что подпрограмма оставила в `HL`.
- `dss_callp(addr, param)` делает то же, но дополнительно передаёт один 16-битный параметр через стек.
- `dss_expath(path, buf, subfunc)` извлекает один компонент пути в `buf`.
- `dss_appinfo(subfunc, buf)` возвращает сведения о текущей программе: параметры, каталог или полный путь.
- `dss_getenv(name, buf)` читает переменную окружения в `buf`.
- `dss_setenv("NAME=VALUE")` создаёт или обновляет переменную окружения.
- `dss_getdisk()` возвращает номер текущего диска (`0 = A:`, `1 = B:`, `2 = C:`...).
- `dss_setdisk(disk)` переключает текущий диск.
- `dss_ei()` / `dss_di()` разрешают или запрещают прерывания.
- `dss_setvmod(mode, page)` переключает DSS-текстовый/видео-режим и активную страницу.
- `dss_getvmod(mode, page)` читает текущий режим DSS и страницу.
- `dss_scroll(x, y, w, h, dir, count)` прокручивает прямоугольную текстовую область.
- `dss_clear(x, y, w, h, color, attr)` очищает прямоугольную текстовую область.

Подфункции для разбора пути:

| Константа | Что возвращает |
|-----------|----------------|
| `EXPATH_ALL` | Нормализованный путь целиком |
| `EXPATH_DRIVE` | Только диск, например `C:` |
| `EXPATH_PATH` | Только каталог |
| `EXPATH_NAME` | Имя файла без расширения |
| `EXPATH_EXT` | Расширение без точки |

Подфункции `APPINFO`:

| Константа | Что возвращает |
|-----------|----------------|
| `APPINFO_PARAMS` | Только параметры командной строки |
| `APPINFO_DIR` | Каталог, где лежит текущий `.EXE` |
| `APPINFO_FULL` | Полный путь к текущему `.EXE` |

Направления прокрутки:

| Константа | Значение |
|-----------|----------|
| `SCROLL_UP` | Прокрутка вверх |
| `SCROLL_DOWN` | Прокрутка вниз |

Примечание по `dss_clear()`: в публичном прототипе сохранены исторические имена `(color, attr)`, но текущая обёртка DSS передаёт пятый аргумент как байт текстового атрибута, а шестой — как символ заполнения. На практике вызовы выглядят так: `dss_clear(4, 8, 34, 6, 0x1F, ' ')`.

## dir.h -- Совместимость с DOS/Turbo C

`<dir.h>` даёт thin-wrapper API поверх DSS:

```c
char *getcwd(char *buf, int size);
int   fnsplit(const char *path, char *drive, char *dir, char *fname, char *ext);
```

- `getcwd(buf, size)` заполняет `buf` текущим каталогом и возвращает `buf` при успехе, либо `NULL` при ошибке. Текущая реализация параметр `size` игнорирует, поэтому буфер должен быть достаточно большим.
- `fnsplit(path, drive, dir, fname, ext)` раскладывает путь на компоненты и возвращает bitmask найденных частей.

Флаги `fnsplit()`:

| Константа | Значение |
|-----------|----------|
| `DRIVE` | Найден диск |
| `DIRECTORY` | Найден каталог |
| `FILENAME` | Найдено имя файла |
| `EXTENSION` | Найдено расширение |
| `WILDCARDS` | Зарезервировано для совместимости DOS/Turbo C |

В этом же заголовке есть алиасы `chdir`, `mkdir`, `rmdir`, `findfirst`, `findnext`, `ffirst`, `fnext`, которые используют DSS-реализацию.

## stdlib.h -- Окружение

Стандартная библиотека SDK также даёт thin-wrapper доступ к переменным окружения:

```c
char *getenv(const char *name);
int   putenv(const char *namevalue);
```

- `getenv(name)` возвращает указатель на внутренний статический буфер, либо `NULL`, если переменной нет.
- `putenv("NAME=VALUE")` вызывает `dss_setenv()` и возвращает `0` при успехе.

## sprinter/bios.h -- Вызовы BIOS

BIOS использует `RST #08` и даёт более низкоуровневый доступ к железу, чем DSS.

```c
void bios_setpal(u8 index, u8 r, u8 g, u8 b);
void bios_putpixel(u16 x, u8 y, u8 color);
u16  bios_version(void);
u8   bios_board_id(void);
u8   inp(u16 port);
void outp(u16 port, u8 value);
```

- `bios_setpal(index, r, g, b)` меняет один цвет палитры. `index` — `0..255`, компоненты `r`, `g`, `b` — `0..63`.
- `bios_putpixel(x, y, color)` рисует один пиксель в графическом режиме.
- `bios_version()` возвращает версию BIOS в BCD-формате.
- `bios_board_id()` возвращает идентификатор платы или типа Sprinter.
- `inp(port)` и `outp(port, value)` — thin-wrapper над инструкциями Z80 `IN` и `OUT`.

После `bios_putpixel()` аппаратный селектор строки VRAM остаётся активным. После прямого рисования пикселей нужно вызвать `video_safe_porty()`, прежде чем снова обращаться к обычной памяти.

## sprinter/video.h -- Видео

### Константы режимов

| Константа | Значение | Описание |
|-----------|----------|----------|
| `VMODE_TEXT40` | 0x02 | Текстовый 40x32 |
| `VMODE_TEXT80` | 0x03 | Текстовый 80x32 |
| `VMODE_ZX` | 0x03 | Синоним `VMODE_TEXT80` |
| `VMODE_320_16` | 0x80 | 320x256, 16 цветов |
| `VMODE_320` | 0x81 | 320x256, 256 цветов |
| `VMODE_640_16` | 0x82 | 640x256, 16 цветов |
| `VMODE_640` | 0x82 | Синоним `VMODE_640_16` |

### Функции

```c
void video_setmode(u8 mode);
u8   video_getmode(void);
void video_swap(void);
void video_sync_enable(void);
void video_sync_disable(void);
void video_vsync(void);
void video_setpal(u8 index, u8 r, u8 g, u8 b);
void video_setpal_range(u8 first, u16 count, const video_rgb6_t *colors);
void video_setpal_range8(u8 first, u16 count, const video_rgb8_t *colors);
void video_setpal_graf(void);
void video_mapvram(u8 win, u8 page);
void video_safe_porty(void);
```

- `video_setmode(mode)` напрямую переключает аппаратный видеорежим.
  При переключении в текстовый режим также отключается аппаратный источник синхронизации.
- `video_getmode()` возвращает текущий байт аппаратного режима.
- `video_swap()` меняет отображаемую страницу для double buffering через бит 0 RGMOD.
- `video_sync_enable()` включает аппаратный источник синхронизации, используемый `video_vsync()`.
- `video_sync_disable()` отключает этот источник синхронизации.
- `video_vsync()` включает аппаратный источник синхронизации через порт Sprinter `#004E`, затем ждёт
  переход бита 5 порта `#FFFE` из 1 в 0. На Sprinter это означает выход из
  нижнего blank/border-интервала `Y > 256` и продолжение с начала кадра. Если sync-бит недоступен, функция
  откатывается к ожиданию одного прерывания, чтобы не зависать. Источник
  синхронизации остаётся включённым для следующих кадров и отключается при `video_setmode(VMODE_TEXT*)`
  или `dss_exit()`.
- `video_setpal(index, r, g, b)` задаёт один цвет палитры через 8-битные RGB-компоненты (`0..255`).
- `video_setpal_range(first, count, colors)` задаёт диапазон цветов через 6-битные RGB-компоненты (`0..63`). `count` может быть до 255.
- `video_setpal_range8(first, count, colors)` задаёт диапазон цветов через 8-битные RGB-компоненты (`0..255`) с масштабированием до аппаратного диапазона.
- `video_setpal_graf()` загружает встроенную BIOS-палитру GRAF.
- `video_mapvram(win, page)` подключает VRAM-страницу в окно памяти `0..3`. Обычно VRAM-страницы начинаются с `0x50`.
- `video_safe_porty()` возвращает `PORT_Y` в безопасное состояние после прямой работы с пикселями.

Константы размеров:

```c
#define SCREEN_W_320    320
#define SCREEN_H_320    256
#define SCREEN_W_640    640
#define SCREEN_H_640    256
#define TEXT_COLS        80
#define TEXT_ROWS        32
```

## sprinter/ay.h -- AY/PT3 музыка

Минимальный слой для запуска PT3 player image из страничной памяти DSS. Бинарный ресурс должен содержать `pt3play.asm`, собранный под адрес `0xC000`, и PT3-модуль сразу после проигрывателя. SDK временно подключает первый лист указанного DSS-блока в `WIN3`, вызывает нужный entrypoint проигрывателя и восстанавливает окно.

```c
#include <sprinter/ay.h>

u8 ay_pt3_init(u8 block);
u8 ay_pt3_play(u8 block);
u8 ay_pt3_mute(u8 block);
```

- `ay_pt3_init(block)` вызывает `START` проигрывателя и инициализирует модуль, лежащий сразу после player code.
- `ay_pt3_play(block)` вызывает `START+5`; его нужно вызывать один раз на кадр, обычно сразу после `video_vsync()` или из кадрового обработчика прерывания.
- `ay_pt3_mute(block)` вызывает `START+8`; используйте перед паузой, выходом или освобождением DSS-блока.
- Все функции возвращают `0` при успехе или код ошибки DSS `SETWIN`, если страницу не удалось подключить.
- Страница с player image должна оставаться выделенной всё время воспроизведения. Перед `dss_freemem(block)` обязательно вызовите `ay_pt3_mute(block)`.

## sprinter/assets.h -- Загрузка runtime-ресурсов

`asset_load_pages()` загружает обычный бинарный файл или упакованный ресурс SDK
в последовательные страницы памяти DSS. Упакованные ресурсы создаёт
`tools/pack_asset.py`: он разбивает файл на чанки по 16 КБ, сжимает их через
`mhmt -hst -zxh` и записывает в контейнер SDK `SPK1`.

```c
#include <sprinter/assets.h>

i16 asset_load_pages(const char *path, u8 block, u8 page_count);
```

Функция возвращает число затронутых страниц назначения или `-1` при ошибке.
Упакованные и обычные файлы используют те же runtime-имена; loader распознаёт
упаковку по заголовку `SPK1`, иначе читает файл постранично как raw. HRUST
depacker работает in-place в WIN3, поэтому обычный стек EXE в WIN2 не
переключается.

## sprinter/gfx.h -- Опциональная графическая библиотека

`gfx.lib` не входит в основную `sprinter.lib` и линкуется только явно. Библиотека рассчитана на режим `320x256x256` и предоставляет общий слой для спрайтов, blit-операций, восстановления фона и графических примитивов.

```c
#include <sprinter/gfx.h>

void gfx_draw_sprite8(u8 screen, u16 x, u8 y, const void *data, u8 flags);
void gfx_draw_sprite16(u8 screen, u16 x, u8 y, const void *data, u8 flags);
void gfx_draw_sprite24(u8 screen, u16 x, u8 y, const void *data, u8 flags);

void gfx_draw_pixel(u8 screen, u16 x, u8 y, u8 color, u8 flags);
void gfx_draw_hline(u8 screen, u16 x, u8 y, u16 width, u8 color, u8 flags);
void gfx_draw_vline(u8 screen, u16 x, u8 y, u8 height, u8 color, u8 flags);
void gfx_draw_line(u8 screen, u16 x0, u8 y0, u16 x1, u8 y1, u8 color, u8 flags);
void gfx_draw_line_thick(u8 screen, u16 x0, u8 y0, u16 x1, u8 y1,
                         u8 thickness, u8 color, u8 flags);
void gfx_draw_rect(u8 screen, u16 x, u8 y, u16 width, u8 height, u8 color, u8 flags);
void gfx_draw_rect_thick(u8 screen, u16 x, u8 y, u16 width, u8 height,
                         u8 thickness, u8 color, u8 flags);
void gfx_fill_rect(u8 screen, u16 x, u8 y, u16 width, u8 height, u8 color, u8 flags);
void gfx_draw_circle(u8 screen, u16 cx, u8 cy, u8 radius, u8 color, u8 flags);

void gfx_restore_rect(u8 screen, u16 x, u8 y, u8 width, u8 height);
void gfx_restore_sprite8(u8 screen, u16 x, u8 y);
void gfx_restore_sprite16(u8 screen, u16 x, u8 y);
void gfx_restore_sprite24(u8 screen, u16 x, u8 y);

void gfx_copy_rect(u8 dst_screen, u8 src_screen, u16 x, u8 y, u8 width, u8 height);
void gfx_blit_rect(u8 dst_screen, u16 dst_x, u8 dst_y,
                   u8 src_screen, u16 src_x, u8 src_y,
                   u8 width, u8 height);
void gfx_scroll_rect(u8 screen, u16 dst_x, u8 dst_y,
                     u16 src_x, u8 src_y, u8 width, u8 height);
void gfx_copy_screen(u8 dst_screen, u8 src_screen);
void gfx_flip(void);

i16 gfx_load_resource_pages(const char *path, u8 first_page, u8 page_count);
u8  gfx_draw_resource(u8 screen, u16 x, u8 y, u8 base_page,
                      const gfx_resource_t *resources, u8 id, u8 flags);
```

`gfx_load_resource_pages()` внутри использует `asset_load_pages()`, поэтому
`.gfx` файлы могут быть обычным выводом `png2gfx.py`/`bmp2gfx.py` или
упакованными `SPK1` файлами SDK, созданными при `PACK_ASSETS=1`.

Основные флаги:
- `GFX_OPAQUE` -- обычное копирование.
- `GFX_MASKED` -- цвет `0xFF` считается прозрачным.
- `GFX_VRAM_ONLY` -- рисовать только в VRAM, не обновляя DRAM-зеркало видеопамяти.

Функции примитивов используют индексы текущей графической палитры и принимают те же аргументы `screen` и `flags`, что и функции спрайтов. Для `gfx_draw_vline()` и `gfx_fill_rect()` значение `height == 0` означает 256 строк, как и в существующем соглашении для полноэкранного копирования.

`gfx_restore_rect()` восстанавливает область VRAM из DRAM-зеркала того же экрана. Это именно аппаратный механизм Sprinter для стирания спрайтов: фон рисуется обычным режимом, временный спрайт рисуется с `GFX_VRAM_ONLY`, а затем сохранённый в DRAM фон копируется обратно в VRAM.

`gfx_copy_rect()` копирует прямоугольник между логическими экранами 0/1 в тех же координатах. Это не отдельная функция восстановления из тени, а копирование между экранами; при этом источник читается из DRAM-зеркала соответствующего экрана. `gfx_blit_rect()` копирует из независимых координат источника в независимые координаты назначения, включая скроллинг внутри одного экрана. `gfx_scroll_rect()` -- удобная обёртка над `gfx_blit_rect()` для одного экрана. Спрайты шириной 16 и 24 пикселя, `gfx_restore_rect()`, `gfx_copy_rect()` и `gfx_blit_rect()` используют аппаратный акселератор Sprinter. Примитивы реализованы как отсечённая прямая запись в VRAM. Для линковки одного примера укажите `EXTRA_LIBS=$(SDK_DIR)build/gfx.lib`; из корня SDK можно собрать архив отдельно командой `make gfx`.

Для функций копирования прямоугольников `height == 0` означает 256 строк. `width == 0` передаётся акселератору как строка шириной 256 байт. `gfx_blit_rect()` копирует через внутренний 256-байтный буфер строки, поэтому горизонтальное перекрытие безопасно; если источник и назначение находятся на одном экране и назначение начинается ниже источника, строки копируются снизу вверх для корректного вертикального скроллинга.

## sprinter/mouse.h -- Драйвер мыши

Драйвер мыши вызывается через `RST #30`.

```c
u8   mouse_init(void);
void mouse_show(void);
void mouse_hide(void);
void mouse_stat(mouse_state_t *state);
void mouse_setpos(u16 x, u16 y);
void mouse_xbound(u16 min_x, u16 max_x);
void mouse_ybound(u16 min_y, u16 max_y);
```

- `mouse_init()` возвращает `1`, если драйвер и мышь доступны, иначе `0`.
- `mouse_show()` / `mouse_hide()` управляют отображением курсора.
- `mouse_stat(state)` заполняет структуру кнопками и координатами.
- `mouse_setpos(x, y)` ставит курсор в заданную точку.
- `mouse_xbound(min_x, max_x)` и `mouse_ybound(min_y, max_y)` ограничивают область движения.

Структура `mouse_state_t`:

```c
typedef struct {
    u8  buttons;
    u16 x;
    u16 y;
} mouse_state_t;
```

Маски кнопок:

```c
#define MS_BTN_LEFT     0x01
#define MS_BTN_RIGHT    0x02
#define MS_BTN_MIDDLE   0x04
```

## sprinter/ports.h -- Аппаратные порты

Константы портов для прямой работы через `inp()` и `outp()`:

### Окна памяти

| Константа | Порт | Окно |
|-----------|------|------|
| `PORT_WIN0` | 0x82 | 0x0000-0x3FFF |
| `PORT_WIN1` | 0xA2 | 0x4000-0x7FFF |
| `PORT_WIN2` | 0xC2 | 0x8000-0xBFFF |
| `PORT_WIN3` | 0xE2 | 0xC000-0xFFFF |

### Видео

| Константа | Порт | Описание |
|-----------|------|----------|
| `PORT_GFXMODE` | 0xC3 | Регистр графического режима |
| `PORT_RGMOD` | 0xC9 | Страница отображения и double buffering |
| `PORT_PAL_ADDR` | 0x89 | Адрес палитры |
| `PORT_PAL_DATA` | 0x89 | Данные палитры |
| `PORT_CBL_DIR` | 0x004E | Управление CBL/Covox-Blaster; 16-битный порт, использовать `OUT (C),A` |

### Звук

| Константа | Порт | Описание |
|-----------|------|----------|
| `PORT_AY_ADDR` | 0x8C | Адрес регистра AY-3-8910 |
| `PORT_AY_WRITE` | 0x8D | Запись данных AY-3-8910 |
| `PORT_AY_READ` | 0x8E | Чтение данных AY-3-8910 |
| `PORT_COVOX` | 0x88 | Вывод Covox DAC |

### Прочее

| Константа | Порт | Описание |
|-----------|------|----------|
| `PORT_KEYB` | 0xFE | ZX-клавиатура и бордюр |
| `PORT_KEMPSTON` | 0x1F | Джойстик Kempston |
| `PORT_MOUSE_BTN` | 0x58 | Кнопки мыши |
| `PORT_CTC0`..`PORT_CTC3` | 0x10-0x13 | Каналы таймера CTC |
| `PORT_SIO_A_DATA` | 0x18 | PS/2 data |
| `PORT_SIO_A_CMD` | 0x19 | PS/2 command |
| `VRAM_PAGE_BASE` | 0x50 | Базовый номер VRAM-страницы |
