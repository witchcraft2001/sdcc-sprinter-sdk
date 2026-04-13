# Установка

## Необходимое ПО

Для работы с SDK нужны три инструмента:

| Инструмент | Версия | Назначение |
|------------|--------|------------|
| **SDCC** | 4.x | Компилятор Си для Z80 |
| **Python** | 3.x | Конвертер ihx2exe.py (Intel HEX в Sprinter EXE) |
| **GNU Make** | любая | Система сборки |

## Установка на macOS

```bash
brew install sdcc python3
```

GNU Make уже входит в состав Xcode Command Line Tools. Если не установлен:

```bash
xcode-select --install
```

## Установка на Linux

### Ubuntu / Debian

```bash
sudo apt install sdcc python3 make
```

### Fedora / RHEL

```bash
sudo dnf install sdcc python3 make
```

### Arch Linux

```bash
sudo pacman -S sdcc python make
```

## Установка на Windows

### Вариант 1: через MSYS2 (рекомендуется)

1. Установите [MSYS2](https://www.msys2.org/)
2. Откройте терминал **MSYS2 MinGW64** и выполните:

```bash
pacman -S mingw-w64-x86_64-sdcc mingw-w64-x86_64-python3 make
```

### Вариант 2: ручная установка

1. Скачайте и установите [Python 3.x](https://www.python.org/downloads/) -- при установке отметьте "Add to PATH"
2. Скачайте и установите [SDCC](https://sdcc.sourceforge.net/) -- Windows installer, добавьте в PATH
3. Скачайте [GNU Make](https://gnuwin32.sourceforge.net/packages/make.htm) или используйте Make из Git Bash

## Проверка установки

После установки убедитесь, что все инструменты доступны:

```bash
sdcc --version
# SDCC 4.x.x ...

python3 --version
# Python 3.x.x

make --version
# GNU Make ...
```

На Windows вместо `python3` может использоваться `python`:

```bash
python --version
```

## Сборка SDK

Клонируйте или распакуйте SDK и выполните сборку:

```bash
cd sdcc-sprinter-sdk
make
```

Эта команда:
1. Компилирует все модули библиотеки (103 файла `.c`) в объектные файлы `.rel`
2. Собирает CRT0 (стартовый код) из `lib/crt0.s`
3. Упаковывает всё в архив `build/sprinter.lib` для селективной линковки

Результат:
```
build/crt0.rel       -- стартовый код
build/sprinter.lib   -- библиотека SDK (103 модуля)
```

## Сборка примеров

```bash
make examples
```

Эта команда собирает все 13 примеров из директории `examples/`. Для каждого примера создаётся исполняемый файл `.exe` в формате Sprinter DSS.

## Сборка sjasmplus (опционально)

Ассемблер **sjasmplus** нужен только для проектов со смешанным кодом C+ASM:

```bash
make tools
```

Это скачает и скомпилирует sjasmplus 1.22 в `tools/bin/sjasmplus`.

## Структура установленного SDK

```
sdcc-sprinter-sdk/
├── Makefile              # Система сборки
├── include/              # Заголовочные файлы
│   ├── stdio.h
│   ├── stdlib.h
│   ├── string.h
│   ├── ctype.h
│   ├── conio.h
│   ├── sprinter.h        # Общий заголовок Sprinter API
│   └── sprinter/
│       ├── dss.h          # DSS API
│       ├── bios.h         # BIOS API
│       ├── video.h        # Видео
│       ├── mouse.h        # Мышь
│       ├── ports.h        # Порты
│       └── types.h        # Типы данных
├── lib/
│   ├── crt0.s             # Стартовый код
│   └── src/               # Исходники библиотеки
├── build/                 # Скомпилированные файлы
├── examples/              # 13 примеров
├── tools/                 # Утилиты (ihx2exe.py)
└── scripts/               # Вспомогательные скрипты
```
