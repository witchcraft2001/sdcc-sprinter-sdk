# Graphics Programming Guide

## Setting Video Mode

Switch to 320x256, 256-color graphics mode:

```c
#include <sprinter.h>

video_setmode(VMODE_320);   /* 320x256, 8 bits per pixel */
```

Available graphics modes:

| Mode | Resolution | Colors |
|------|-----------|--------|
| `VMODE_320_16` (0x80) | 320x256 | 16 |
| `VMODE_320` (0x81) | 320x256 | 256 |
| `VMODE_640_16` (0x82) | 640x256 | 16 |

To return to text mode when your program finishes:

```c
video_setmode(VMODE_TEXT80);  /* 80x32 text mode */
```

Always restore text mode before exiting, or the DSS prompt will be unusable.

## Setting the Palette

### Method 1: BIOS #A4 (individual entries)

Use `bios_setpal()` to set individual palette entries:

```c
/*  index, red, green, blue -- each component is 0-63 (6-bit) */
bios_setpal(0, 0, 0, 0);       /* Color 0: black */
bios_setpal(1, 63, 0, 0);      /* Color 1: bright red */
bios_setpal(2, 0, 63, 0);      /* Color 2: bright green */
bios_setpal(3, 0, 0, 63);      /* Color 3: bright blue */
bios_setpal(255, 63, 63, 63);  /* Color 255: white */
```

### Method 2: BIOS #A6 (preset palette)

The BIOS provides a built-in "GRAF" palette (256 organized colors). This must be set via inline assembly:

```c
static void set_graf_palette(void) {
    __asm
        push    ix
        ld      b, #1       ; Palette preset 1 (GRAF)
        ld      e, #0
        xor     a
        ld      c, #0xA6    ; BIOS function: Set preset palette
        rst     #0x08
        pop     ix
    __endasm;
}
```

### Method 3: video_setpal()

The `video_setpal()` function uses 0-255 range per component (it scales internally):

```c
video_setpal(0, 0, 0, 0);         /* Black */
video_setpal(1, 255, 0, 0);       /* Red */
video_setpal(2, 0, 255, 0);       /* Green */
```

### Palette Ranges

For a shared graphics palette, it is more practical to load several entries at once:

```c
static const video_rgb6_t palette[] = {
    {0, 0, 0},
    {63, 0, 0},
    {0, 63, 0},
    {0, 0, 63}
};

video_setpal_range(0, 4, palette);
video_setpal_graf();          /* built-in GRAF palette */
```

`video_setpal_range()` takes hardware 6-bit components (`0..63`), while `video_setpal_range8()` takes normal 8-bit RGB components (`0..255`) and scales them. This API lives in the base `sprinter.lib`, so it can be shared by `gfx.lib` and future geometry primitive layers.

### Method 4: video_setpal256_fast() (full 256-colour update)

For full-screen palette swaps and fade effects, `video_setpal256_fast()` loads all 256 entries into both hardware palette pages in a single call. It builds a hardware-format buffer on the stack and issues four BIOS PIC_SET_PAL range calls instead of 256 individual `bios_setpal()` calls, which makes it fast enough to use on every frame:

```c
static const video_rgb6_t my_palette[256] = { /* 256 RGB6 entries */ };

video_setpal256_fast(my_palette);
```

The function temporarily uses about 1 KB of stack, restores `PORT_Y` to its safe value, and is safe to call in any video mode. Use it whenever you need to install or refresh a whole 256-entry palette - in particular it is the path the [fade module](fade_guide.md) uses under the hood.

## Drawing Pixels

Use `bios_putpixel()` to draw individual pixels:

```c
bios_putpixel(x, y, color);
```

- `x` -- horizontal position, 0-319 (type `u16`)
- `y` -- vertical position, 0-255 (type `u8`)
- `color` -- palette index, 0-255 (type `u8`)

**Example -- draw a white horizontal line:**

```c
u16 x;
for (x = 0; x < 320; x++) {
    bios_putpixel(x, 128, 255);
}
```

**Example -- draw a filled rectangle:**

```c
u16 x;
u8  y;
for (y = 10; y < 50; y++) {
    for (x = 20; x < 100; x++) {
        bios_putpixel(x, y, 5);
    }
}
```

## IMPORTANT: Call video_safe_porty() After Drawing

The `bios_putpixel()` function uses the PORT_Y hardware register to select the VRAM row for writing. If PORT_Y is left pointing to a row in the visible VRAM area after drawing, subsequent memory accesses (code fetch, stack operations, data reads) can corrupt video memory.

**You MUST call `video_safe_porty()` after all pixel drawing is complete:**

```c
/* Draw everything */
for (y = 0; y < 256; y++) {
    for (x = 0; x < 320; x++) {
        bios_putpixel(x, y, (u8)(x + y));
    }
}

/* Reset PORT_Y to safe zone -- REQUIRED! */
video_safe_porty();

/* Now it is safe to do other things */
dss_waitkey();
```

Forgetting this call is a common source of mysterious screen corruption.

## VRAM Structure

In 320x256, 256-color mode, each pixel is one byte. Each screen row occupies 1024 bytes (1 KB) in VRAM, even though only 320 bytes are visible. The VRAM is organized as pages:

- VRAM pages start at page number `0x50` (`VRAM_PAGE_BASE`)
- Each page is 16 KB (one memory window)
- The PORT_Y register selects which row is mapped into the current VRAM window

The `bios_putpixel()` function handles all the PORT_Y and VRAM page management internally, so you do not need to manage it manually for simple drawing.

### Direct VRAM Access (advanced)

For faster drawing, you can map VRAM pages directly into a memory window and write bytes:

```c
/* Map VRAM page into window 3 (0xC000-0xFFFF) */
video_mapvram(3, VRAM_PAGE_BASE);

/* Write pixel at (x, y) -- row must be selected via PORT_Y first */
/* This is what bios_putpixel does internally */
```

Direct VRAM access is significantly faster than `bios_putpixel()` for bulk operations (filling screens, blitting sprites), but requires careful PORT_Y management.

## Double Buffering

The Sprinter supports double buffering for tear-free animation:

```c
/* Draw to back buffer */
/* ... drawing code ... */

video_safe_porty();

/* Wait for vertical blank */
video_vsync();

/* Swap front and back buffers */
video_swap();
```

The `video_swap()` function toggles the display buffer via bit 0 of the RGMOD port (`#C9`). While the screen shows one buffer, you draw into the other. `video_vsync()` uses the Sprinter hardware sync bit on `#FFFE.5` and returns at the start of a frame, so animated examples should call it immediately before `gfx_flip()`/`video_swap()`. Static examples do not need per-frame sync unless they flip visible pages.

## Optional gfx.lib

Sprites, screen copies, and background restore are provided by the separate `build/gfx.lib` archive. It is not linked automatically and does not increase EXE size unless used:

```make
EXTRA_LIBS = $(SDK_DIR)build/gfx.lib
```

The library targets `VMODE_320` only and uses the same palette configured through `video_setpal*()`.

```c
#include <sprinter/gfx.h>

gfx_draw_sprite8(GFX_SCREEN_0, 40, 40, sprite8, GFX_MASKED);
gfx_draw_sprite16(GFX_SCREEN_0, 80, 40, sprite16, GFX_OPAQUE);
gfx_draw_sprite24(GFX_SCREEN_1, 120, 40, sprite24, GFX_MASKED);

gfx_restore_sprite16(GFX_SCREEN_0, 80, 40);
gfx_copy_screen(GFX_SCREEN_1, GFX_SCREEN_0);
gfx_blit_rect(GFX_SCREEN_0, 32, 40, GFX_SCREEN_0, 32, 48, 128, 64);
gfx_flip();
```

Fixed sprite sizes are 8x8, 16x16, and 24x24. Color `0xFF` is transparent when `GFX_MASKED` is set. 16x16 and 24x24 sprites, rectangle copies, and background restore use the Sprinter hardware accelerator.

Restore works by copying from the DRAM mirror of the same screen back to VRAM. To draw something temporarily without updating the saved background, use `GFX_VRAM_ONLY`, then call `gfx_restore_rect()` or one of the `gfx_restore_sprite*()` helpers. `gfx_copy_rect()` copies between screens at the same coordinates; `gfx_blit_rect()` copies between arbitrary source and destination coordinates and is suitable for accelerated scrolling.

### Graphics Primitives

`gfx.lib` also provides simple primitives that use the same palette and screen arguments:

```c
gfx_draw_pixel(GFX_SCREEN_0, 10, 20, 15, GFX_OPAQUE);
gfx_draw_line(GFX_SCREEN_0, 20, 30, 160, 90, 8, GFX_OPAQUE);
gfx_draw_line_thick(GFX_SCREEN_0, 20, 100, 220, 40, 3, 4, GFX_OPAQUE);
gfx_draw_rect(GFX_SCREEN_0, 12, 18, 296, 220, 15, GFX_OPAQUE);
gfx_draw_rect_thick(GFX_SCREEN_0, 24, 32, 80, 40, 2, 10, GFX_OPAQUE);
gfx_fill_rect(GFX_SCREEN_0, 20, 204, 80, 22, 1, GFX_OPAQUE);
gfx_draw_circle(GFX_SCREEN_0, 260, 190, 24, 14, GFX_OPAQUE);
```

Lines are clipped to the 320x256 screen. `gfx_draw_line_thick()` uses a square brush with the requested thickness, so diagonal thick lines look like a stepped band. `gfx_fill_rect()` and `gfx_draw_vline()` treat `height == 0` as 256 rows, which is useful for full-screen operations. Use `GFX_VRAM_ONLY` for temporary primitives that should later be removed by restoring from the DRAM mirror.

The `20_gfxst` example demonstrates the primitive layer together with sprites: a white screen border, a red filled rectangle, a thin gray line, a thick blue diagonal line, a white circle, and four X/cross sprites.

## PNG Resources

Graphics can be embedded as C arrays or prepared from PNG files before compilation:

```bash
python3 tools/png2gfx.py \
    --out gfxdemo.gfx \
    --header res.h \
    --name demo_gfx \
    assets/sprite16.png assets/sprite24.png
```

The tool accepts non-interlaced 8-bit RGB/RGBA/indexed PNGs, builds one shared palette with up to 255 opaque colors, uses `0xFF` as transparent pixels, and writes a page-friendly `.gfx` file where resource payloads do not cross 16 KB page boundaries. The generated header contains `video_rgb6_t *_palette[]` and `gfx_resource_t *_resources[]`.

## BMP Resources

For 8-bit indexed BMP files, use `tools/bmp2gfx.py`. It supports full images
through `--image` and sprite sheets through `--sheet PATH:WxH`:

```bash
python3 tools/bmp2gfx.py \
    --out ppong.gfx \
    --header res.h \
    --name ppong \
    --image assets/bg0.bmp \
    --image assets/bg1.bmp \
    --sheet assets/sprites.bmp:16x16 \
    --sheet-transparent-index 255
```

When `--palette` is omitted, the tool builds one shared palette from all input
images and sheet tiles. The transparent sheet index is excluded from the palette.
The limit is 255 opaque colors; exceeding it stops the build with
`combined palette exceeds 255 colors`.

For legacy or fixed-palette workflows, pass `--palette file.bmp`. In that mode,
the specified BMP provides the palette and all other images are remapped to the
nearest colors from that palette.

At runtime, load the file into paged memory and pass the base page plus resource id when drawing:

```c
page = dss_getmem();
gfx_load_resource_pages("GFXDEMO.GFX", page, 1);
video_setpal_range(0, DEMO_GFX_PALETTE_COUNT, demo_gfx_palette);
gfx_draw_resource(GFX_SCREEN_0, 96, 80, page, demo_gfx_resources, 0, 0);
```

Resource files may be stored raw or packed. To build packed example resources,
run:

```bash
make examples PACK_ASSETS=1
```

The build keeps the same DOS 8.3 filenames and replaces the file contents with
an SDK `SPK1` container. `tools/pack_asset.py` compresses each 16 KB chunk with
the local `mhmt -hst -zxh` tool and verifies it with `mhmt -d`. At runtime,
`gfx_load_resource_pages()` detects `SPK1` and depacks into the destination DSS
pages; raw `.gfx` files still load the same way.

Use 8.3 filenames for all runtime resources. DSS cannot open long filenames, so generated files such as `GFXDEMO.GFX` and generated headers such as `res.h` intentionally use short names.

## Complete Graphics Example

```c
#include <sprinter.h>

static void set_graf_palette(void) {
    __asm
        push    ix
        ld      b, #1
        ld      e, #0
        xor     a
        ld      c, #0xA6
        rst     #0x08
        pop     ix
    __endasm;
}

void main(void) {
    u8 y;
    u16 x;

    /* Enter graphics mode */
    video_setmode(VMODE_320);
    set_graf_palette();

    /* Draw a color gradient */
    for (y = 0; y < 200; y++) {
        for (x = 0; x < 320; x++) {
            bios_putpixel(x, y + 28, (u8)((x + (u16)y) & 255));
        }
    }

    /* Draw a white border */
    for (x = 0; x < 320; x++) {
        bios_putpixel(x, 27, 255);
        bios_putpixel(x, 228, 255);
    }
    for (y = 27; y <= 228; y++) {
        bios_putpixel(0, y, 255);
        bios_putpixel(319, y, 255);
    }

    /* MUST call after all drawing! */
    video_safe_porty();

    /* Wait for key press */
    dss_waitkey();

    /* Return to text mode */
    video_setmode(VMODE_TEXT80);
}
```

## Performance Tips

1. **Minimize bios_putpixel() calls.** Each call involves a BIOS RST, IX save/restore, and PORT_Y management. For large fills, consider direct VRAM access.

2. **Draw in row order.** PORT_Y is set per row. Drawing pixels in row order avoids redundant PORT_Y changes.

3. **Use the GRAF palette.** The built-in BIOS #A6 palette provides 256 well-organized colors without needing to define each one.

4. **Always restore text mode.** Call `video_setmode(VMODE_TEXT80)` before exiting to leave the screen in a usable state.
