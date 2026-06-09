#!/usr/bin/env python3
"""Convert small PNG sprites into ZX Sprinter gfx resources.

The output binary is page-friendly: generated resource offsets never cross a
16 KB page boundary, so runtime code can map page_delta and use offset directly.
Supported PNGs are non-interlaced 8-bit indexed, RGB, or RGBA images.
"""

import argparse
import os
import struct
import zlib

PNG_SIG = b"\x89PNG\r\n\x1a\n"
PAGE_SIZE = 16384
TRANSPARENT = 0xFF


def paeth(a, b, c):
    p = a + b - c
    pa = abs(p - a)
    pb = abs(p - b)
    pc = abs(p - c)
    if pa <= pb and pa <= pc:
        return a
    if pb <= pc:
        return b
    return c


def read_png(path):
    with open(path, "rb") as f:
        data = f.read()
    if not data.startswith(PNG_SIG):
        raise ValueError("%s: not a PNG file" % path)

    pos = len(PNG_SIG)
    width = height = bit_depth = color_type = interlace = None
    plte = []
    trns = b""
    idat = []

    while pos < len(data):
        length = struct.unpack(">I", data[pos:pos + 4])[0]
        ctype = data[pos + 4:pos + 8]
        chunk = data[pos + 8:pos + 8 + length]
        pos += 12 + length

        if ctype == b"IHDR":
            width, height, bit_depth, color_type, _, _, interlace = struct.unpack(">IIBBBBB", chunk)
        elif ctype == b"PLTE":
            plte = [tuple(chunk[i:i + 3]) for i in range(0, len(chunk), 3)]
        elif ctype == b"tRNS":
            trns = chunk
        elif ctype == b"IDAT":
            idat.append(chunk)
        elif ctype == b"IEND":
            break

    if bit_depth != 8:
        raise ValueError("%s: only 8-bit PNGs are supported" % path)
    if interlace:
        raise ValueError("%s: interlaced PNGs are not supported" % path)
    if color_type not in (2, 3, 6):
        raise ValueError("%s: only RGB, RGBA, and indexed PNGs are supported" % path)

    bpp = {2: 3, 3: 1, 6: 4}[color_type]
    raw = zlib.decompress(b"".join(idat))
    stride = width * bpp
    rows = []
    prev = bytearray(stride)
    p = 0

    for _ in range(height):
        ftype = raw[p]
        p += 1
        row = bytearray(raw[p:p + stride])
        p += stride

        for i in range(stride):
            left = row[i - bpp] if i >= bpp else 0
            up = prev[i]
            upper_left = prev[i - bpp] if i >= bpp else 0
            if ftype == 1:
                row[i] = (row[i] + left) & 0xFF
            elif ftype == 2:
                row[i] = (row[i] + up) & 0xFF
            elif ftype == 3:
                row[i] = (row[i] + ((left + up) >> 1)) & 0xFF
            elif ftype == 4:
                row[i] = (row[i] + paeth(left, up, upper_left)) & 0xFF
            elif ftype != 0:
                raise ValueError("%s: unknown PNG filter %d" % (path, ftype))

        rows.append(bytes(row))
        prev = row

    pixels = []
    for row in rows:
        for x in range(width):
            if color_type == 3:
                idx = row[x]
                alpha = trns[idx] if idx < len(trns) else 255
                rgb = plte[idx]
            elif color_type == 2:
                rgb = tuple(row[x * 3:x * 3 + 3])
                alpha = 255
            else:
                off = x * 4
                rgb = tuple(row[off:off + 3])
                alpha = row[off + 3]
            pixels.append(None if alpha < 128 else rgb)

    return width, height, pixels


def convert_images(paths):
    palette = []
    palette_map = {}
    images = []

    for path in paths:
        width, height, pixels = read_png(path)
        out = bytearray()
        for rgb in pixels:
            if rgb is None:
                out.append(TRANSPARENT)
                continue
            if rgb not in palette_map:
                if len(palette) >= 255:
                    raise ValueError("palette exceeds 255 opaque colors")
                palette_map[rgb] = len(palette)
                palette.append(rgb)
            out.append(palette_map[rgb])
        images.append((path, width, height, bytes(out)))

    return palette, images


def pack_binary(palette, images):
    table_size = len(images) * 8
    header = bytearray()
    header += b"GFX1"
    header += struct.pack("<HH", len(images), len(palette))
    header += bytes(rgb[i] for rgb in palette for i in range(3))
    data_pos = len(header) + table_size
    payload = bytearray(b"\0" * data_pos)
    entries = []

    for _, width, height, pixels in images:
        if data_pos % PAGE_SIZE + len(pixels) > PAGE_SIZE:
            data_pos += PAGE_SIZE - (data_pos % PAGE_SIZE)
            if len(payload) < data_pos:
                payload.extend(b"\0" * (data_pos - len(payload)))
        page_delta = data_pos // PAGE_SIZE
        offset = data_pos % PAGE_SIZE
        entries.append((page_delta, offset, len(pixels), width, height, 0))
        payload.extend(pixels)
        data_pos += len(pixels)

    table = bytearray()
    for entry in entries:
        table += struct.pack("<BHHBBB", *entry)
    payload[len(header):len(header) + table_size] = table
    return bytes(payload), entries


def c_ident(name):
    out = []
    for ch in name:
        out.append(ch if ch.isalnum() else "_")
    ident = "".join(out).strip("_")
    if not ident or ident[0].isdigit():
        ident = "gfx_" + ident
    return ident


def write_header(path, name, palette, entries):
    guard = "_" + c_ident(name).upper() + "_H"
    with open(path, "w", newline="\n") as f:
        f.write("#ifndef %s\n#define %s\n\n" % (guard, guard))
        f.write("#include <sprinter/gfx.h>\n\n")
        f.write("#define %s_RESOURCE_COUNT %d\n" % (name.upper(), len(entries)))
        f.write("#define %s_PALETTE_COUNT %d\n\n" % (name.upper(), len(palette)))
        f.write("static const video_rgb8_t %s_palette[] = {\n" % name)
        for rgb in palette:
            r, g, b = rgb[0], rgb[1], rgb[2]
            f.write("    {%d, %d, %d},\n" % (r, g, b))
        f.write("};\n\n")
        f.write("static const gfx_resource_t %s_resources[] = {\n" % name)
        for page_delta, offset, size, width, height, flags in entries:
            f.write("    {%d, 0x%04X, %d, %d, %d, %d},\n" %
                    (page_delta, offset, size, width, height, flags))
        f.write("};\n\n#endif /* %s */\n" % guard)


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("--out", required=True, help="output .gfx binary")
    parser.add_argument("--header", required=True, help="generated C header")
    parser.add_argument("--name", default="gfx_assets", help="C identifier prefix")
    parser.add_argument("png", nargs="+")
    args = parser.parse_args()

    name = c_ident(args.name)
    palette, images = convert_images(args.png)
    binary, entries = pack_binary(palette, images)

    os.makedirs(os.path.dirname(args.out) or ".", exist_ok=True)
    with open(args.out, "wb") as f:
        f.write(binary)
    write_header(args.header, name, palette, entries)


if __name__ == "__main__":
    main()
