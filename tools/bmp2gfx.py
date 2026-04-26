#!/usr/bin/env python3
"""Convert 8-bit BMP images and sprite sheets into Sprinter gfx resources."""

import argparse
import os
import struct

PAGE_SIZE = 16384
TRANSPARENT = 0xFF


def read_bmp8(path):
    with open(path, "rb") as f:
        data = f.read()

    if data[:2] != b"BM":
        raise ValueError("%s: not a BMP file" % path)

    pixel_offset = struct.unpack_from("<I", data, 10)[0]
    dib_size = struct.unpack_from("<I", data, 14)[0]
    if dib_size < 40:
        raise ValueError("%s: unsupported BMP DIB header" % path)

    width, height = struct.unpack_from("<ii", data, 18)
    planes = struct.unpack_from("<H", data, 26)[0]
    bpp = struct.unpack_from("<H", data, 28)[0]
    compression = struct.unpack_from("<I", data, 30)[0]
    colors = struct.unpack_from("<I", data, 46)[0] or 256

    if planes != 1 or bpp != 8 or compression != 0:
        raise ValueError("%s: only uncompressed 8-bit BMP files are supported" % path)
    if width <= 0 or height == 0:
        raise ValueError("%s: invalid BMP dimensions" % path)

    palette = []
    for i in range(colors):
        b, g, r, _ = struct.unpack_from("BBBB", data, 14 + dib_size + i * 4)
        palette.append((r, g, b))

    abs_height = abs(height)
    row_stride = ((width * bpp + 31) // 32) * 4
    rows = []
    for y in range(abs_height):
        start = pixel_offset + y * row_stride
        rows.append(data[start:start + width])
    if height > 0:
        rows.reverse()

    return width, abs_height, palette, rows


def rgb8_to_rgb6(rgb):
    return (rgb[0] >> 2, rgb[1] >> 2, rgb[2] >> 2)


def color_dist(a, b):
    dr = a[0] - b[0]
    dg = a[1] - b[1]
    db = a[2] - b[2]
    return dr * dr + dg * dg + db * db


def nearest_index(rgb, palette, allow_255):
    best = 0
    best_dist = None
    limit = len(palette) if allow_255 else min(len(palette), 255)

    for i in range(limit):
        d = color_dist(rgb, palette[i])
        if best_dist is None or d < best_dist:
            best = i
            best_dist = d
    return best


def parse_sheet(spec):
    try:
        path, size = spec.rsplit(":", 1)
        w, h = size.lower().split("x", 1)
        return path, int(w), int(h)
    except ValueError:
        raise ValueError("sheet spec must be PATH:WxH")


def parse_transparent(value, rows):
    if value == "none":
        return None
    if value == "auto":
        return rows[0][0]
    return int(value, 0) & 0xFF


def convert_rows(rows, src_palette, dst_palette, transparent_index):
    out = bytearray()
    allow_255 = transparent_index is None

    for row in rows:
        for idx in row:
            if transparent_index is not None and idx == transparent_index:
                out.append(TRANSPARENT)
            else:
                out.append(nearest_index(src_palette[idx], dst_palette, allow_255))
    return bytes(out)


def slice_rows(rows, x, y, width, height):
    return [row[x:x + width] for row in rows[y:y + height]]


def pack_binary(palette, entries):
    table_size = len(entries) * 9
    header = bytearray()
    header += b"GFX2"
    header += struct.pack("<HH", len(entries), len(palette))
    header += bytes(rgb8_to_rgb6(rgb)[i] for rgb in palette for i in range(3))

    data_pos = len(header) + table_size
    payload = bytearray(b"\0" * data_pos)
    payload[:len(header)] = header
    packed_entries = []

    for width, height, pixels, flags in entries:
        page_delta = data_pos // PAGE_SIZE
        offset = data_pos % PAGE_SIZE
        size = len(pixels)
        packed_entries.append((page_delta, offset, size, width, height, flags))
        payload.extend(pixels)
        data_pos += size

    table = bytearray()
    for entry in packed_entries:
        table += struct.pack("<BHHHBB", *entry)
    payload[len(header):len(header) + table_size] = table
    return bytes(payload), packed_entries


def c_ident(name):
    out = []
    for ch in name:
        out.append(ch if ch.isalnum() else "_")
    ident = "".join(out).strip("_")
    if not ident or ident[0].isdigit():
        ident = "gfx_" + ident
    return ident


def write_header(path, name, palette, entries, page_count):
    guard = "_" + c_ident(name).upper() + "_H"
    upper = name.upper()

    with open(path, "w", newline="\n") as f:
        f.write("#ifndef %s\n#define %s\n\n" % (guard, guard))
        f.write("#include <sprinter/gfx.h>\n\n")
        f.write("#define %s_IMAGE_COUNT %d\n" % (upper, len(entries)))
        f.write("#define %s_PALETTE_COUNT %d\n" % (upper, len(palette)))
        f.write("#define %s_PAGE_COUNT %d\n\n" % (upper, page_count))
        f.write("static const video_rgb6_t %s_palette[] = {\n" % name)
        for rgb in palette:
            r, g, b = rgb8_to_rgb6(rgb)
            f.write("    {%d, %d, %d},\n" % (r, g, b))
        f.write("};\n\n")
        f.write("static const gfx_image_t %s_images[] = {\n" % name)
        for page_delta, offset, size, width, height, flags in entries:
            f.write("    {%d, 0x%04X, %d, %d, %d, %d},\n" %
                    (page_delta, offset, size, width, height, flags))
        f.write("};\n\n#endif /* %s */\n" % guard)


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("--out", required=True)
    parser.add_argument("--header", required=True)
    parser.add_argument("--name", default="bmp_gfx")
    parser.add_argument("--palette", required=True, help="8-bit BMP whose palette is used for all entries")
    parser.add_argument("--image", action="append", default=[], help="full BMP image to add")
    parser.add_argument("--sheet", action="append", default=[], help="sprite sheet BMP as PATH:WxH")
    parser.add_argument("--sheet-transparent-index", default="auto",
                        help="auto, none, or numeric BMP palette index")
    args = parser.parse_args()

    name = c_ident(args.name)
    _, _, dst_palette, _ = read_bmp8(args.palette)
    entries = []

    for path in args.image:
        width, height, src_palette, rows = read_bmp8(path)
        pixels = convert_rows(rows, src_palette, dst_palette, None)
        entries.append((width, height, pixels, 0))

    for spec in args.sheet:
        path, tile_w, tile_h = parse_sheet(spec)
        width, height, src_palette, rows = read_bmp8(path)
        transparent_index = parse_transparent(args.sheet_transparent_index, rows)

        for y in range(0, height, tile_h):
            for x in range(0, width, tile_w):
                tile = slice_rows(rows, x, y, tile_w, tile_h)
                pixels = convert_rows(tile, src_palette, dst_palette, transparent_index)
                entries.append((tile_w, tile_h, pixels, 1))

    if not entries:
        raise ValueError("no images were provided")

    binary, packed_entries = pack_binary(dst_palette, entries)
    page_count = (len(binary) + PAGE_SIZE - 1) // PAGE_SIZE

    os.makedirs(os.path.dirname(args.out) or ".", exist_ok=True)
    with open(args.out, "wb") as f:
        f.write(binary)
    write_header(args.header, name, dst_palette, packed_entries, page_count)


if __name__ == "__main__":
    main()
