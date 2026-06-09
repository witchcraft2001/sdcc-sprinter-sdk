# Руководство по fade-палитре

SDK содержит модуль (`<sprinter/fade.h>`) для плавного fade-in / fade-out активной 256-цветной палитры без потери музыки и пользовательской логики. Модуль построен из двух слоёв:

- **пошаговое ядро**, которое применяет ровно один палитра-шаг за вызов и сразу возвращает управление, и
- несколько **блокирующих обёрток** для простых демонстраций и старого стиля кода.

Главный API — пошаговый: он не ждёт vsync и не проигрывает музыку, поэтому фрейм-цикл целиком в руках вызывающего кода.

## Зачем пошаговый API

`video_setpal_range()` вызывает `bios_setpal()` для каждого цвета — этого недостаточно, чтобы делать 256-цветный fade на 50 Гц во время воспроизведения PT3. Модуль fade:

1. один раз копирует исходную палитру в RAM,
2. пересчитывает 64-байтную LUT яркости для текущего шага (один умножитель на компонент, пересчёт только при смене шага),
3. разворачивает LUT в аппаратный буфер на стеке и
4. отправляет результат в обе аппаратные страницы через `video_setpal256_fast()` (четыре BIOS range-вызова, никаких 256 индивидуальных вызовов).

Один шаг укладывается существенно быстрее кадра 50 Гц, поэтому PT3-проигрыватель и пользовательская логика не страдают.

## Краткое описание API

```c
#include <sprinter/fade.h>

#define FADE_MODE_OUT   0
#define FADE_MODE_IN    1
#define FADE_STEPS      32

typedef struct fade_state {
    u8  active;
    u8  duration_frames;
    u8  mode;
    u8  frame;
    u8  step;
    u16 accumulator;
} fade_state_t;

void fade_capture_palette(const video_rgb8_t *palette);

u8   fade_begin(fade_state_t *state, u8 duration_frames, u8 mode);
u8   fade_step(fade_state_t *state);
void fade_cancel(fade_state_t *state);

void fade_out(u8 frames);
void fade_in(u8 frames);
void fade_out_music(u8 frames, u8 pt3_block);
void fade_in_music(u8 frames, u8 pt3_block);
```

### `fade_capture_palette(palette)`

Копирует 256 RGB8-цветов (`0..255` на канал) во внутренний буфер. Вызывается один раз после установки конечной палитры в железо (например, через `video_setpal256_fast()` или `video_setpal_range()`). Все последующие `fade_in()` / `fade_out()` берут эту палитру как «максимальную яркость».

### `fade_begin(state, duration_frames, mode)`

Инициализирует `state` для пошагового fade.

- `duration_frames` — общее число вызовов `fade_step()`. Значение `0` сразу применяет конечное состояние (чёрная палитра для `FADE_MODE_OUT`, исходная для `FADE_MODE_IN`) и оставляет `state->active = 0`.
- `mode` — `FADE_MODE_OUT` или `FADE_MODE_IN`.

Возвращает `state->active` (1 — остались шаги, 0 — fade уже завершён).

### `fade_step(state)`

Применяет ровно один шаг палитры и возвращает управление. **Не** ждёт vsync и **не** проигрывает музыку. Вызывающий сам решает, когда дёрнуть `video_vsync()`, когда вызвать `ay_pt3_play()` и какую ещё работу выполнить. Возвращает 1, пока fade активен, и 0, когда применён последний шаг.

### `fade_cancel(state)`

Прекращает fade без изменения текущей палитры — пригодится, если игрок нажал «skip».

### Блокирующие обёртки

`fade_out`, `fade_in`, `fade_out_music`, `fade_in_music` — тонкие удобные обёртки над `fade_begin` / `fade_step`. Они сами ждут vsync и при необходимости вызывают `ay_pt3_play(pt3_block)`, чтобы PT3 не прерывался. Используйте их для splash-экранов и коротких заставок; для геймплея предпочтительнее пошаговый API.

## Рекомендуемый шаблон с PT3

```c
fade_state_t fade;

video_setpal256_fast(my_palette);
fade_capture_palette(my_palette);
ay_pt3_init(music_block);

fade_begin(&fade, 16, FADE_MODE_OUT);
while (fade.active) {
    video_vsync();
    ay_pt3_play(music_block);
    /* обновление состояния, ввод, спрайты, ... */
    fade_step(&fade);
}
```

Каждая итерация — один vsync, один такт музыки, один шаг fade и ваша логика, в нужном порядке. Поскольку `fade_step()` отдаёт управление быстро, PT3 не «проседает», а игровой код по-прежнему получает время во время fade.

## Память

Модуль держит в статической памяти одну захваченную палитру (768 байт) и 64-байтную LUT компонент. Аппаратный буфер для BIOS строится на стеке внутри asm-помощника и не занимает статической RAM. Закладывайте ~1 КБ свободного стека на время вызова `fade_step()`.

## См. также

- `<sprinter/video.h>` — `video_setpal256_fast()` (`docs/ru/06_graphics_guide.md`, «video_setpal256_fast()»)
- `examples/29_fade/` — полный пример с PT3, анимацией бордюра и пошаговым fade
