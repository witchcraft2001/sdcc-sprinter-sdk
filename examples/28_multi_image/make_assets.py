#!/usr/bin/env python3
"""Generate five full-screen demo images for the multi-image example.

Each image is written as a raw 320x256 8-bit blob (5 EMM pages). All images
share a single palette emitted to res.h, so the video palette is set once and
images can be flipped without reprogramming colours.
"""

import math
import os

W, H = 320, 256
PAGE_SIZE = 0x4000
PALETTE_COUNT_NEEDED = 0  # set after PAL8 is built


# 8-bit RGB palette; shifted to 6-bit in the generated header.
PAL8 = [
    (0,   0,   0),     # 0 - black
    (255, 255, 255),   # 1 - white
    (32,  40,  120),   # 2 - splash bg (deep blue)
    (40,  90,  50),    # 3 - menu bg (dark green)
    (60,  60,  60),    # 4 - loading bg (gray)
    (200, 160, 30),    # 5 - won bg (gold)
    (140, 30,  30),    # 6 - over bg (dark red)
    (220, 220, 100),   # 7 - won accent
    (255, 200, 200),   # 8 - over accent
    (180, 200, 255),   # 9 - splash accent
    (180, 240, 180),   # 10 - menu accent
    (200, 200, 200),   # 11 - loading accent
]


DIGITS = {
    '1': [
        "  X  ",
        " XX  ",
        "X X  ",
        "  X  ",
        "  X  ",
        "  X  ",
        "XXXXX",
    ],
    '2': [
        " XXX ",
        "X   X",
        "    X",
        "  XX ",
        " X   ",
        "X    ",
        "XXXXX",
    ],
    '3': [
        " XXX ",
        "X   X",
        "    X",
        "  XX ",
        "    X",
        "X   X",
        " XXX ",
    ],
    '4': [
        "X   X",
        "X   X",
        "X   X",
        "XXXXX",
        "    X",
        "    X",
        "    X",
    ],
    '5': [
        "XXXXX",
        "X    ",
        "XXXX ",
        "    X",
        "    X",
        "X   X",
        " XXX ",
    ],
}


def fill(color):
    return bytearray(bytes([color]) * (W * H))


def border(buf, color, thickness=4):
    for t in range(thickness):
        for x in range(W):
            buf[t * W + x] = color
            buf[(H - 1 - t) * W + x] = color
        for y in range(H):
            buf[y * W + t] = color
            buf[y * W + (W - 1 - t)] = color


def draw_digit(buf, digit, color, scale=20):
    glyph = DIGITS[digit]
    gw = 5 * scale
    gh = 7 * scale
    x0 = (W - gw) // 2
    y0 = (H - gh) // 2
    for gy in range(7):
        row = glyph[gy]
        for gx in range(5):
            if row[gx] != 'X':
                continue
            base_x = x0 + gx * scale
            base_y = y0 + gy * scale
            for py in range(scale):
                yoff = (base_y + py) * W
                for px in range(scale):
                    buf[yoff + base_x + px] = color


def make_splash():
    buf = fill(2)
    for y in range(H):
        for x in range(W):
            if ((x + y) // 12) % 4 == 0:
                buf[y * W + x] = 9
    draw_digit(buf, '1', 1)
    border(buf, 1)
    return buf


def make_menu():
    buf = fill(3)
    for y in range(0, H, 32):
        for yy in range(y, min(y + 6, H)):
            row = yy * W
            for x in range(W):
                buf[row + x] = 10
    draw_digit(buf, '2', 1)
    border(buf, 1)
    return buf


def make_loading():
    buf = fill(4)
    for x in range(0, W, 32):
        for xx in range(x, min(x + 6, W)):
            for y in range(H):
                buf[y * W + xx] = 11
    draw_digit(buf, '3', 1)
    border(buf, 1)
    return buf


def make_won():
    buf = fill(5)
    cx, cy = W // 2, H // 2
    for deg in range(0, 360, 12):
        rad = deg * math.pi / 180.0
        dx = math.cos(rad)
        dy = math.sin(rad)
        for t in range(0, 200, 1):
            x = int(cx + dx * t)
            y = int(cy + dy * t)
            if 0 <= x < W and 0 <= y < H:
                buf[y * W + x] = 7
    draw_digit(buf, '4', 1)
    border(buf, 1)
    return buf


def make_over():
    buf = fill(6)
    for i in range(H):
        for t in range(-3, 4):
            x = i * W // H + t
            if 0 <= x < W:
                buf[i * W + x] = 8
            x = (W - 1) - i * W // H + t
            if 0 <= x < W:
                buf[i * W + x] = 8
    draw_digit(buf, '5', 1)
    border(buf, 1)
    return buf


IMAGES = [
    ("splash.gfx",  "SPLASH.GFX",  "splash",  make_splash),
    ("menu.gfx",    "MENU.GFX",    "menu",    make_menu),
    ("loading.gfx", "LOADING.GFX", "loading", make_loading),
    ("won.gfx",     "WON.GFX",     "won",     make_won),
    ("over.gfx",    "OVER.GFX",    "over",    make_over),
]


def emit_header(path):
    pages_per_image = (W * H + PAGE_SIZE - 1) // PAGE_SIZE
    with open(path, "w", newline="\n") as f:
        f.write("#ifndef _MULTI_IMG_H\n#define _MULTI_IMG_H\n\n")
        f.write("#include <sprinter/gfx.h>\n\n")
        f.write("#define MULTI_IMG_COUNT       %d\n" % len(IMAGES))
        f.write("#define MULTI_IMG_PAGES       %d\n" % pages_per_image)
        f.write("#define MULTI_IMG_PALETTE_COUNT %d\n\n" % len(PAL8))

        f.write("static const video_rgb6_t multi_palette[] = {\n")
        for r, g, b in PAL8:
            f.write("    {%d, %d, %d},\n" % (r >> 2, g >> 2, b >> 2))
        f.write("};\n\n")

        f.write("static const char *const multi_paths[MULTI_IMG_COUNT] = {\n")
        for _, dos_name, _, _ in IMAGES:
            f.write("    \"%s\",\n" % dos_name)
        f.write("};\n\n")

        f.write("static const char *const multi_names[MULTI_IMG_COUNT] = {\n")
        for _, _, name, _ in IMAGES:
            f.write("    \"%s\",\n" % name)
        f.write("};\n\n")

        # size > GFX_PAGE_SIZE (0x4000) forces the slow path off the small-
        # sprite fast lane; the full-screen origin-aligned dispatch in
        # gfx_draw_image_page picks the column-major asm helper because of
        # the GFX_RESF_COLUMNS flag below. gfx_image_t.height is u8; 0 means
        # full 256 rows.
        h_field = 0 if H == 256 else H
        flag = "GFX_RESF_COLUMNS" if USE_COLUMNS else "0"
        f.write("static const gfx_image_t multi_images[MULTI_IMG_COUNT] = {\n")
        for _ in IMAGES:
            f.write("    {0, 0x0000, 0x4001, %d, %d, %s},\n"
                    % (W, h_field, flag))
        f.write("};\n\n")

        f.write("#endif /* _MULTI_IMG_H */\n")


def transpose_to_columns(buf):
    """Reorder a row-major WxH buffer into column-major (column c at offset
    c*H, contiguous H bytes). With H=256 every column maps to a clean
    256-byte stride, so columns never cross a 16K page boundary."""
    out = bytearray(W * H)
    for c in range(W):
        col_off = c * H
        for r in range(H):
            out[col_off + r] = buf[r * W + c]
    return out


USE_COLUMNS = True    # column-major source feeds the vertical-copy accel path


def main():
    out_dir = os.path.dirname(os.path.abspath(__file__))
    for filename, _dos_name, _name, fn in IMAGES:
        path = os.path.join(out_dir, filename)
        data = fn()
        if USE_COLUMNS:
            data = transpose_to_columns(data)
        with open(path, "wb") as f:
            f.write(bytes(data))
    emit_header(os.path.join(out_dir, "res.h"))


if __name__ == "__main__":
    main()
