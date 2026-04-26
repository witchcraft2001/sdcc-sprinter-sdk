# PPONG Assets

Current `ppong.gfx` is generated from local placeholder art by `make_assets.py`.
The assets are intentionally simple and may be replaced later without changing
the game code if the replacement resource keeps the same entry order:

1. `0`: 320x128 background top half.
2. `1`: 320x128 background bottom half.
3. `2`: 16x16 ball.
4. `3`: 16x16 paddle top.
5. `4`: 16x16 paddle middle.
6. `5`: 16x16 paddle bottom.
7. `6+`: 16x16 font glyphs in `font_chars` order.

Free asset candidates checked for later replacement:

- OpenGameArt, "Pong Pixel Art" by ElectDraw, license CC0.
- OpenGameArt, "Paddle and ball" by Aj_, license CC0.
- OpenGameArt, "NES Pong Tiles" by HonkeyKong, license CC0.
- OpenGameArt, "Jelly 16x16 font" by shiru8bit, license CC0 or CC-BY 3.0.
- OpenGameArt, "Space background" by drakzlin, license CC0.

Keep all deployed filenames DOS 8.3 compatible: `PPONG.EXE`, `PPONG.GFX`.

## Required Source Format

`tools/bmp2gfx.py` currently expects uncompressed 8-bit indexed BMP files.
For this example, `bmp2gfx.py` builds one shared palette from all provided
background and sprite images. The build fails if the combined opaque color set
exceeds 255 colors.

Runtime transparency uses palette index `255` in `assets/sprites.bmp`. Pixels
with index `255` are converted to the SDK transparent color `0xFF` and are not
counted as palette colors.

Expected source files:

- `assets/bg0.bmp`: top half of the background, exactly `320x128`, 8-bit BMP.
- `assets/bg1.bmp`: bottom half of the background, exactly `320x128`, 8-bit BMP.
- `assets/sprites.bmp`: sprite sheet, 8-bit BMP, tile size `16x16`.

The full game screen is `320x256`. A single `320x256` background is not stored
as one resource because the current `gfx_image_t.size` field is 16-bit, while
`320*256 = 81920` bytes. Two `320x128` resources keep each image below `65535`
bytes and work with the existing resource format.

## Sprite Sheet Order

Keep the tile order in `assets/sprites.bmp` stable unless you also update the
resource constants in `main.c`.

Tile order:

1. `0`: ball, `16x16`.
2. `1`: paddle top segment, `16x16`.
3. `2`: paddle middle segment, `16x16`.
4. `3`: paddle bottom segment, `16x16`.
5. `4+`: font glyphs, `16x16` each.

Font glyph order must match `font_chars` in `main.c`:

```text
ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789:-. 
```

The final character is a space. If you add more characters, append them to both
the sheet and `font_chars`; if you reorder characters, text rendering will draw
the wrong glyphs.

## Replacing With Your Own Assets

Recommended workflow:

1. Edit `make_assets.py` so it copies or converts your source art into:
   `assets/bg0.bmp`, `assets/bg1.bmp`, and `assets/sprites.bmp`.
2. Keep those generated files out of git unless you explicitly want to commit
   the generated BMPs. The local `.gitignore` excludes `assets/`.
3. Run the normal example build; it will regenerate `res.h` and `PPONG.GFX`.

Color requirements:

- Keep the combined opaque color count of `bg0.bmp`, `bg1.bmp`, and
  `sprites.bmp` at `255` or lower.
- Reserve sprite-sheet palette index `255` for transparency.
- If the build fails with `combined palette exceeds 255 colors`, reduce or
  quantize the source art before converting it.

Manual workflow for quick local testing:

1. Run `python3 make_assets.py` once to create `assets/`.
2. Replace `assets/bg0.bmp`, `assets/bg1.bmp`, and/or `assets/sprites.bmp`.
3. Run `make` in `examples/24_ppong` without `clean`, otherwise the generator
   will recreate the placeholder files.

For committed SDK changes, prefer the recommended workflow and make
`make_assets.py` deterministic so a clean checkout can rebuild the same assets.

## Rebuild Commands

From the SDK root:

```sh
make -C examples/24_ppong SDK_DIR=$(pwd)/ SDCC290_BIN_DIR=/Users/dmitry/dev/zx/sdcc2
scripts/make_floppy.sh
scripts/deploy_img_local.sh
```

For a full SDK rebuild:

```sh
make SDCC290_BIN_DIR=/Users/dmitry/dev/zx/sdcc2
make examples SDCC290_BIN_DIR=/Users/dmitry/dev/zx/sdcc2
scripts/make_floppy.sh
scripts/deploy_img_local.sh
```

The deployed files on the target image are `PPONG.EXE` and `PPONG.GFX`.

## CC0 Notes

CC0 assets may normally be used, modified, and redistributed without attribution.
Still keep source links, author names, and license names in this file or a
similar asset manifest. That makes the asset provenance auditable even when the
license does not require attribution.
