#!/usr/bin/env python3
import os
import struct
import zlib


PALETTE = (
    (255, 32, 32),
    (255, 160, 32),
    (255, 240, 64),
    (64, 224, 96),
    (64, 192, 255),
    (144, 96, 255),
    (255, 96, 192),
    (255, 255, 255),
)


def write_png(path, size, seed):
    rows = []
    for y in range(size):
        row = bytearray()
        for x in range(size):
            border = x == 0 or y == 0 or x == size - 1 or y == size - 1
            cross = abs(x - y) <= 1 or abs((size - 1 - x) - y) <= 1
            if border or cross:
                row += bytes(PALETTE[(x + y + seed) & 7] + (255,))
            else:
                row += bytes((0, 0, 0, 0))
        rows.append(b"\x00" + bytes(row))

    raw = zlib.compress(b"".join(rows))

    def chunk(name, data):
        body = name + data
        return struct.pack(">I", len(data)) + body + struct.pack(">I", zlib.crc32(body) & 0xFFFFFFFF)

    png = b"\x89PNG\r\n\x1a\n"
    png += chunk(b"IHDR", struct.pack(">IIBBBBB", size, size, 8, 6, 0, 0, 0))
    png += chunk(b"IDAT", raw)
    png += chunk(b"IEND", b"")

    with open(path, "wb") as f:
        f.write(png)


os.makedirs("assets", exist_ok=True)
write_png("assets/sprite16.png", 16, 10)
write_png("assets/sprite24.png", 24, 40)
