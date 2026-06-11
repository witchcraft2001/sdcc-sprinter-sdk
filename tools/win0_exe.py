#!/usr/bin/env python3
"""win0_exe.py - pack a PRELOAD .EXE for the extended WIN0..WIN2 layout (4b).

Builds:  [512-byte header][stage-1 loader @0x8100][6-byte size table]
         [WIN0 blob][WIN1 blob][WIN2 blob]

The header is the standard 512 bytes (OFFCOD=512), with LOADER = loader size so
DSS loads only the loader, leaves the file open positioned at the size table, and
jumps. The PRELOAD loader (lib/win0/loader.c) reads the table then streams the
three blobs straight into freshly GETMEM'd pages P0/P1/P2.

Blob bases are FIXED: WIN0=0x0180, WIN1=0x4000, WIN2=0x8000 (the loader places
them at P0:0x0180, P1:0x4000, P2:0x8000). Empty regions yield length 0.
(0x0000-0x017F in P0 holds vectors/boot params/cmd line, set up at runtime.)

Usage: win0_exe.py <loader.ihx> <payload.ihx> <out.exe>
"""
import sys
import struct

HDR = 512
LD_ADDR = 0x8100
SP_REG = 0xBFFF


def parse_ihx(path):
    mem = {}
    for line in open(path):
        line = line.strip()
        if not line.startswith(":"):
            continue
        n = int(line[1:3], 16)
        a = int(line[3:7], 16)
        t = int(line[7:9], 16)
        d = line[9:9 + 2 * n]
        if t == 0:
            for i in range(n):
                mem[a + i] = int(d[2 * i:2 * i + 2], 16)
    return mem


def region(mem, base, end):
    """Bytes [base, end) as a 0xFF-padded blob anchored at base (len 0 if none)."""
    sel = {a: v for a, v in mem.items() if base <= a < end}
    if not sel:
        return b""
    top = max(sel)
    return bytes(sel.get(base + i, 0xFF) for i in range(top - base + 1))


def main():
    if len(sys.argv) != 4:
        sys.exit("usage: win0_exe.py <loader.ihx> <payload.ihx> <out.exe>")
    loader = parse_ihx(sys.argv[1])
    payload = parse_ihx(sys.argv[2])

    llo = min(loader)
    if llo != LD_ADDR:
        sys.exit("loader must start at 0x%04X, got 0x%04X" % (LD_ADDR, llo))
    lcode = bytes(loader.get(llo + i, 0xFF) for i in range(max(loader) - llo + 1))

    b0 = region(payload, 0x0180, 0x4000)
    b1 = region(payload, 0x4000, 0x8000)
    b2 = region(payload, 0x8000, 0x10000)
    if not b2:
        sys.exit("payload has no WIN2 (trampoline) region at 0x8000")
    if len(lcode) + 6 > LD_ADDR - 0x80:
        sys.exit("loader too big (%d bytes) - would collide with cmd line" % len(lcode))

    hdr = bytearray(HDR)
    hdr[0:3] = b"EXE"
    hdr[3] = 1
    struct.pack_into("<I", hdr, 4, HDR)            # OFFCOD = header size
    struct.pack_into("<H", hdr, 8, len(lcode))     # LOADER = loader size
    struct.pack_into("<H", hdr, 16, LD_ADDR)
    struct.pack_into("<H", hdr, 18, LD_ADDR)       # PC
    struct.pack_into("<H", hdr, 20, SP_REG)        # SP

    table = struct.pack("<HHH", len(b0), len(b1), len(b2))

    with open(sys.argv[3], "wb") as f:
        f.write(hdr + lcode + table + b0 + b1 + b2)
    print("win0_exe: loader=%d  win0=%d win1=%d win2=%d  (code %.1fK)"
          % (len(lcode), len(b0), len(b1), len(b2), (len(b0) + len(b1)) / 1024.0))


if __name__ == "__main__":
    main()
