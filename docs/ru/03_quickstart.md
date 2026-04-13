# Быстрый старт

## Минимальная программа через stdio.h

Самый простой способ написать программу -- использовать стандартную библиотеку Си:

```c
#include <stdio.h>

void main(void) {
    printf("Hello, Sprinter!\n");
    getchar();
}
```

Эта программа выводит строку на экран и ждёт нажатия клавиши. Используются стандартные функции `printf` и `getchar`, знакомые по любому учебнику Си.

## Минимальная программа через Sprinter API

Для минимального размера программы используйте прямые вызовы DSS:

```c
#include <sprinter.h>

void main(void) {
    dss_puts("Hello, Sprinter!\r\n");
    dss_waitkey();
}
```

Заголовок `<sprinter.h>` подключает все API-заголовки Спринтера. Функция `dss_puts` вызывает DSS напрямую, минуя stdio. Результат: **681 байт** вместо ~2.8 КБ с printf.

> **Обратите внимание:** при использовании `dss_puts` перевод строки обозначается `\r\n` (как в DSS/CP/M), а в `printf` достаточно `\n`.

## Создание Makefile

Создайте файл `Makefile` в каталоге вашего проекта:

```makefile
APP      = hello
SRCS     = main.c
SDK_DIR  = /path/to/sdcc-sprinter-sdk/
include $(SDK_DIR)examples/common.mk
```

Параметры:
- `APP` -- имя программы (без расширения). Результат: `hello.exe`
- `SRCS` -- список исходных файлов `.c`
- `SDK_DIR` -- путь к корню SDK (с завершающим `/`)

Для проекта из нескольких файлов:

```makefile
APP      = myapp
SRCS     = main.c utils.c graphics.c
SDK_DIR  = /path/to/sdcc-sprinter-sdk/
include $(SDK_DIR)examples/common.mk
```

## Сборка и запуск

### Сборка

```bash
make
```

Результат -- файл `hello.exe` в текущем каталоге.

### Запуск на эмуляторе

Скопируйте файл `.exe` на виртуальный диск эмулятора и выполните из командной строки DSS:

```
A:\>HELLO.EXE
```

### Запуск на реальном железе

Скопируйте `.exe` на SD-карту или дискету и запустите из DSS.

## Процесс сборки

Сборка проходит в три этапа:

```
main.c  ──[SDCC]──>  main.rel  ──[SDCC линковщик]──>  app.ihx  ──[ihx2exe.py]──>  app.exe
```

### Этап 1: компиляция (.c -> .rel)

SDCC компилирует каждый файл `.c` в объектный файл `.rel`:

```bash
sdcc -mz80 --max-allocs-per-node 5000 --opt-code-speed -I<SDK>/include -c -o _build/main.rel main.c
```

### Этап 2: линковка (.rel -> .ihx)

SDCC линкует объектные файлы вместе с CRT0 и библиотекой:

```bash
sdcc -mz80 --no-std-crt0 --code-loc 0x8100 --data-loc 0x4000 \
    crt0.rel main.rel -lsprinter.lib -o app.ihx
```

Ключевые параметры:
- `--no-std-crt0` -- используется собственный стартовый код
- `--code-loc 0x8100` -- код размещается с адреса 0x8100 (после заголовка EXE)
- `--data-loc 0x4000` -- данные размещаются с адреса 0x4000 (WIN1)
- `-lsprinter.lib` -- подключение библиотеки SDK (селективная линковка)

Формат `.ihx` -- Intel HEX, текстовое представление бинарных данных.

### Этап 3: конвертация (.ihx -> .exe)

Скрипт `ihx2exe.py` создаёт исполняемый файл формата DSS:

```bash
python3 tools/ihx2exe.py app.ihx app.exe
```

Формат EXE DSS:
- 512 байт -- заголовок (сигнатура, адреса загрузки, размер)
- далее -- код программы (загружается в память с адреса 0x8100)

## Очистка

Для удаления результатов сборки:

```bash
make clean
```
