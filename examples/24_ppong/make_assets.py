#!/usr/bin/env python3
import os
import struct


W, H = 320, 256
TILE = 16
TRANSPARENT = 255
FONT_CHARS = "ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789:-. "

PAL = [(0, 0, 0)] * 256
PAL[0] = (10, 18, 32)
PAL[1] = (22, 36, 64)
PAL[2] = (34, 54, 96)
PAL[3] = (58, 92, 144)
PAL[4] = (90, 148, 196)
PAL[5] = (170, 230, 255)
PAL[6] = (255, 255, 255)
PAL[7] = (240, 210, 80)
PAL[8] = (235, 90, 70)
PAL[9] = (80, 220, 150)
PAL[10] = (16, 26, 46)
PAL[11] = (40, 68, 112)
PAL[12] = (120, 180, 220)
PAL[13] = (18, 108, 148)
PAL[14] = (12, 58, 86)
PAL[15] = (150, 240, 210)
PAL[TRANSPARENT] = (255, 0, 255)

GLYPHS = {
    "A": ("01110", "10001", "10001", "11111", "10001", "10001", "10001"),
    "B": ("11110", "10001", "10001", "11110", "10001", "10001", "11110"),
    "C": ("01111", "10000", "10000", "10000", "10000", "10000", "01111"),
    "D": ("11110", "10001", "10001", "10001", "10001", "10001", "11110"),
    "E": ("11111", "10000", "10000", "11110", "10000", "10000", "11111"),
    "F": ("11111", "10000", "10000", "11110", "10000", "10000", "10000"),
    "G": ("01111", "10000", "10000", "10011", "10001", "10001", "01111"),
    "H": ("10001", "10001", "10001", "11111", "10001", "10001", "10001"),
    "I": ("11111", "00100", "00100", "00100", "00100", "00100", "11111"),
    "J": ("00111", "00010", "00010", "00010", "10010", "10010", "01100"),
    "K": ("10001", "10010", "10100", "11000", "10100", "10010", "10001"),
    "L": ("10000", "10000", "10000", "10000", "10000", "10000", "11111"),
    "M": ("10001", "11011", "10101", "10101", "10001", "10001", "10001"),
    "N": ("10001", "11001", "10101", "10011", "10001", "10001", "10001"),
    "O": ("01110", "10001", "10001", "10001", "10001", "10001", "01110"),
    "P": ("11110", "10001", "10001", "11110", "10000", "10000", "10000"),
    "Q": ("01110", "10001", "10001", "10001", "10101", "10010", "01101"),
    "R": ("11110", "10001", "10001", "11110", "10100", "10010", "10001"),
    "S": ("01111", "10000", "10000", "01110", "00001", "00001", "11110"),
    "T": ("11111", "00100", "00100", "00100", "00100", "00100", "00100"),
    "U": ("10001", "10001", "10001", "10001", "10001", "10001", "01110"),
    "V": ("10001", "10001", "10001", "10001", "10001", "01010", "00100"),
    "W": ("10001", "10001", "10001", "10101", "10101", "10101", "01010"),
    "X": ("10001", "10001", "01010", "00100", "01010", "10001", "10001"),
    "Y": ("10001", "10001", "01010", "00100", "00100", "00100", "00100"),
    "Z": ("11111", "00001", "00010", "00100", "01000", "10000", "11111"),
    "0": ("01110", "10001", "10011", "10101", "11001", "10001", "01110"),
    "1": ("00100", "01100", "00100", "00100", "00100", "00100", "01110"),
    "2": ("01110", "10001", "00001", "00010", "00100", "01000", "11111"),
    "3": ("11110", "00001", "00001", "01110", "00001", "00001", "11110"),
    "4": ("00010", "00110", "01010", "10010", "11111", "00010", "00010"),
    "5": ("11111", "10000", "10000", "11110", "00001", "00001", "11110"),
    "6": ("01110", "10000", "10000", "11110", "10001", "10001", "01110"),
    "7": ("11111", "00001", "00010", "00100", "01000", "01000", "01000"),
    "8": ("01110", "10001", "10001", "01110", "10001", "10001", "01110"),
    "9": ("01110", "10001", "10001", "01111", "00001", "00001", "01110"),
    ":": ("00000", "00100", "00100", "00000", "00100", "00100", "00000"),
    "-": ("00000", "00000", "00000", "11111", "00000", "00000", "00000"),
    ".": ("00000", "00000", "00000", "00000", "00000", "00100", "00100"),
    " ": ("00000", "00000", "00000", "00000", "00000", "00000", "00000"),
}


def put(buf, w, h, x, y, c):
    if 0 <= x < w and 0 <= y < h:
        buf[y][x] = c


def rect(buf, w, h, x, y, rw, rh, c):
    for yy in range(y, y + rh):
        for xx in range(x, x + rw):
            put(buf, w, h, xx, yy, c)


def write_bmp(path, width, height, rows):
    stride = (width + 3) & ~3
    pixel_size = stride * height
    header_size = 14 + 40 + 256 * 4
    with open(path, "wb") as f:
        f.write(b"BM")
        f.write(struct.pack("<IHHI", header_size + pixel_size, 0, 0, header_size))
        f.write(struct.pack("<IiiHHIIiiII", 40, width, height, 1, 8, 0,
                            pixel_size, 2835, 2835, 256, 0))
        for r, g, b in PAL:
            f.write(bytes((b, g, r, 0)))
        pad = bytes(stride - width)
        for row in reversed(rows):
            f.write(bytes(row))
            f.write(pad)


def make_bg():
    rows = [[0 for _ in range(W)] for _ in range(H)]
    for y in range(H):
        for x in range(W):
            if y < 32:
                c = 10 if ((x // 8 + y // 8) & 1) else 1
            else:
                c = 0 if ((x * 3 + y * 5) & 63) else 2
                if x == W // 2 or x == W // 2 - 1:
                    c = 3 if (y // 8) & 1 else 11
            rows[y][x] = c
    rect(rows, W, H, 0, 30, W, 2, 5)
    return rows


def make_tile():
    return [[TRANSPARENT for _ in range(TILE)] for _ in range(TILE)]


def ball_tile():
    t = make_tile()
    for y in range(TILE):
        for x in range(TILE):
            dx = x - 7.5
            dy = y - 7.5
            if dx * dx + dy * dy <= 54:
                t[y][x] = 7
            if dx * dx + dy * dy <= 38:
                t[y][x] = 12
            if 4 <= x <= 7 and 3 <= y <= 6:
                t[y][x] = 6
    return t


def paddle_tile(kind):
    t = make_tile()
    shade = {"top": 9, "mid": 4, "bot": 8}[kind]
    for y in range(TILE):
        for x in range(3, 13):
            t[y][x] = shade
        t[y][3] = 15
        t[y][12] = 13
    if kind == "top":
        for x in range(4, 12):
            t[0][x] = 15
            t[1][x] = 15
    if kind == "bot":
        for x in range(4, 12):
            t[14][x] = 14
            t[15][x] = 14
    return t


def glyph_tile(ch):
    t = make_tile()
    glyph = GLYPHS[ch]
    for gy, line in enumerate(glyph):
        for gx, bit in enumerate(line):
            if bit == "1":
                x0 = 3 + gx * 2
                y0 = 1 + gy * 2
                rect(t, TILE, TILE, x0, y0, 2, 2, 6)
    return t


def make_sheet():
    tiles = [ball_tile(), paddle_tile("top"), paddle_tile("mid"), paddle_tile("bot")]
    tiles += [glyph_tile(ch) for ch in FONT_CHARS]
    cols = 16
    rows_count = (len(tiles) + cols - 1) // cols
    rows = [[TRANSPARENT for _ in range(cols * TILE)] for _ in range(rows_count * TILE)]
    for i, tile in enumerate(tiles):
        ox = (i % cols) * TILE
        oy = (i // cols) * TILE
        for y in range(TILE):
            for x in range(TILE):
                rows[oy + y][ox + x] = tile[y][x]
    return cols * TILE, rows_count * TILE, rows


os.makedirs("assets", exist_ok=True)
bg = make_bg()
write_bmp("assets/bg0.bmp", W, H // 2, bg[:H // 2])
write_bmp("assets/bg1.bmp", W, H // 2, bg[H // 2:])
sw, sh, sheet = make_sheet()
write_bmp("assets/sprites.bmp", sw, sh, sheet)
