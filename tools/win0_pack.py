#!/usr/bin/env python3
"""win0_pack.py - split a payload .ihx into WIN0 and WIN2 blobs for the
extended WIN0..WIN2 layout (docs/ru/11_extended_layout.md, Phase 2).

The payload is linked with code in WIN0 (origin 0x0300) and the runtime
trampolines in WIN2 (origin 0xA000). This tool reads the Intel-HEX output,
splits the byte image at 0x8000, and emits a C header with two byte arrays
plus their load addresses and lengths, to be embedded in the stage-1 loader.

Usage: win0_pack.py <payload.ihx> <out.h>
"""
import sys

SPLIT = 0x8000


def parse_ihx(path):
    mem = {}
    base = 0
    for line in open(path):
        line = line.strip()
        if not line.startswith(":"):
            continue
        n = int(line[1:3], 16)
        addr = int(line[3:7], 16)
        typ = int(line[7:9], 16)
        data = line[9:9 + 2 * n]
        if typ == 0x00:
            for i in range(n):
                mem[base + addr + i] = int(data[2 * i:2 * i + 2], 16)
        elif typ == 0x04:
            base = int(data, 16) << 16
        elif typ == 0x01:
            break
    return mem


def blob(mem, lo_ok):
    sel = {a: v for a, v in mem.items() if lo_ok(a)}
    if not sel:
        return 0, 0, []
    lo = min(sel)
    hi = max(sel)
    return lo, hi - lo + 1, [sel.get(lo + i, 0xFF) for i in range(hi - lo + 1)]


def emit(f, name, base, length, arr):
    f.write("#define %s_ADDR 0x%04X\n" % (name, base))
    f.write("#define %s_LEN %d\n" % (name, length))
    f.write("static const unsigned char %s[%d] = {\n" % (name.lower(), length if length else 1))
    if not length:
        f.write("  0x00\n")
    for i in range(0, length, 16):
        f.write("  " + ",".join("0x%02X" % b for b in arr[i:i + 16]) + ",\n")
    f.write("};\n\n")


def main():
    if len(sys.argv) != 3:
        sys.exit("usage: win0_pack.py <payload.ihx> <out.h>")
    mem = parse_ihx(sys.argv[1])
    b0, l0, a0 = blob(mem, lambda a: a < SPLIT)
    b2, l2, a2 = blob(mem, lambda a: a >= SPLIT)
    with open(sys.argv[2], "w", newline="\n") as f:
        f.write("#ifndef PAYLOAD_BLOBS_H\n#define PAYLOAD_BLOBS_H\n\n")
        emit(f, "PAYLOAD_WIN0", b0, l0, a0)
        emit(f, "PAYLOAD_WIN2", b2, l2, a2)
        f.write("#endif /* PAYLOAD_BLOBS_H */\n")
    print("win0_pack: WIN0 %d bytes @0x%04X, WIN2 %d bytes @0x%04X"
          % (l0, b0, l2, b2))


if __name__ == "__main__":
    main()
