#!/usr/bin/env python3
"""Generate the 256-colour gradient image and palette used by the fade demo.

Produces a 320x256 8-bit raw image where each pixel encodes a smooth gradient
across all 256 palette indices. The image is stored column-major so it can be
drawn through gfx_draw_image_page() with the column-major accelerator path.

A matching res.h is emitted with the palette (256 RGB6 entries) and a
gfx_image_t descriptor.
"""

import math
import os

W, H = 320, 256
PAGE_SIZE = 0x4000


def build_palette():
    """A 256-colour HSV-style rainbow that exercises every component."""
    pal = []
    for i in range(256):
        # Sweep hue across the full spectrum; gentle saturation/value curve to
        # keep the fade visually obvious in every part of the image.
        hue = i / 256.0
        sat = 0.85
        val = 0.30 + 0.70 * (math.sin(i * math.pi / 256.0))
        r, g, b = hsv_to_rgb(hue, sat, val)
        pal.append((to6(r), to6(g), to6(b)))
    return pal


def hsv_to_rgb(h, s, v):
    if s == 0.0:
        return v, v, v
    i = int(h * 6.0)
    f = h * 6.0 - i
    p = v * (1.0 - s)
    q = v * (1.0 - s * f)
    t = v * (1.0 - s * (1.0 - f))
    i %= 6
    if i == 0: return v, t, p
    if i == 1: return q, v, p
    if i == 2: return p, v, t
    if i == 3: return p, q, v
    if i == 4: return t, p, v
    return v, p, q


def to6(c):
    n = int(c * 63.0 + 0.5)
    if n < 0: return 0
    if n > 63: return 63
    return n


def make_image():
    buf = bytearray(W * H)
    for y in range(H):
        for x in range(W):
            # Diagonal sweep so every column shows a smooth gradient and the
            # fade is visible regardless of which strip the eye locks onto.
            v = (x + y * 2) & 0xFF
            buf[y * W + x] = v
    # Carve a centred banner of high-contrast text so the brightness change is
    # easy to read during the fade.
    draw_banner(buf, "FADE", 96, 100, 32, 12)
    return buf


def draw_banner(buf, text, x0, y0, scale, ink):
    glyphs = {
        'F': [
            "XXXX",
            "X   ",
            "X   ",
            "XXX ",
            "X   ",
            "X   ",
            "X   ",
        ],
        'A': [
            " XX ",
            "X  X",
            "X  X",
            "XXXX",
            "X  X",
            "X  X",
            "X  X",
        ],
        'D': [
            "XXX ",
            "X  X",
            "X  X",
            "X  X",
            "X  X",
            "X  X",
            "XXX ",
        ],
        'E': [
            "XXXX",
            "X   ",
            "X   ",
            "XXX ",
            "X   ",
            "X   ",
            "XXXX",
        ],
    }
    gw = 4 * scale
    gh = 7 * scale
    cx = x0
    cy = y0
    for ch in text:
        glyph = glyphs[ch]
        for gy in range(7):
            row = glyph[gy]
            for gx in range(4):
                if row[gx] != 'X':
                    continue
                bx = cx + gx * scale
                by = cy + gy * scale
                for py in range(scale):
                    yy = by + py
                    if yy < 0 or yy >= H: continue
                    base = yy * W
                    for px in range(scale):
                        xx = bx + px
                        if 0 <= xx < W:
                            buf[base + xx] = ink
        cx += gw + scale


def transpose_to_columns(buf):
    out = bytearray(W * H)
    for c in range(W):
        col_off = c * H
        for r in range(H):
            out[col_off + r] = buf[r * W + c]
    return out


def emit_header(path, pal):
    pages = (W * H + PAGE_SIZE - 1) // PAGE_SIZE
    with open(path, "w", newline="\n") as f:
        f.write("#ifndef _FADE_RES_H\n#define _FADE_RES_H\n\n")
        f.write("#include <sprinter/gfx.h>\n\n")
        f.write("#define FADE_IMG_PAGES         %d\n" % pages)
        f.write("#define FADE_PALETTE_COUNT     %d\n\n" % len(pal))

        f.write("static const video_rgb6_t fade_palette[%d] = {\n" % len(pal))
        for r, g, b in pal:
            f.write("    {%d, %d, %d},\n" % (r, g, b))
        f.write("};\n\n")

        # height==0 → use full 256-row frame; GFX_RESF_COLUMNS picks the
        # column-major accelerator path.
        f.write("static const gfx_image_t fade_image = "
                "{0, 0x0000, 0x4001, %d, 0, GFX_RESF_COLUMNS};\n\n" % W)
        f.write("#endif /* _FADE_RES_H */\n")


def main():
    here = os.path.dirname(os.path.abspath(__file__))
    pal = build_palette()
    img = make_image()
    img = transpose_to_columns(img)
    with open(os.path.join(here, "fade.gfx"), "wb") as f:
        f.write(bytes(img))
    emit_header(os.path.join(here, "res.h"), pal)


if __name__ == "__main__":
    main()
