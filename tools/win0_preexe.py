#!/usr/bin/env python3
"""win0_preexe.py - build a PRELOAD .EXE to verify DSS loader-field semantics.

DSS (Estex) reads a 128-byte header, then reads LOADER bytes from file offset
128 into LD_ADDR, leaves the file open positioned at 128+LOADER, and jumps. The
running loader can DSS.Read the appended payload from the still-open handle.

This tool emits:  [128-byte header][loader code @0x8100][appended blob]
with header LOADER = loader size, OFFCOD = 128, LD_ADDR=PC=0x8100, SP=0xBFFF.

Usage: win0_preexe.py <loader.ihx> <out.exe> [hdrsize]
"""
import sys
import struct

# Header length is self-described via OFFCOD (offset 4-7). It must be large
# enough that the command-line area below LD_ADDR cannot overrun the loader;
# use the standard 512 (a short 128-byte header is unsafe for long cmd lines).
HDR_DEFAULT = 512
BLOB = b"PRELOAD via DSS.Read OK!\r\n\x00"


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


def main():
    if len(sys.argv) < 3:
        sys.exit("usage: win0_preexe.py <loader.ihx> <out.exe> [hdrsize]")
    hdrsize = int(sys.argv[3]) if len(sys.argv) > 3 else HDR_DEFAULT
    mem = parse_ihx(sys.argv[1])
    lo, hi = min(mem), max(mem)
    if lo != 0x8100:
        sys.exit("loader must start at 0x8100, got 0x%04X" % lo)
    code = bytes(mem.get(lo + i, 0xFF) for i in range(hi - lo + 1))
    blob = BLOB + b"\x00" * (32 - len(BLOB))

    hdr = bytearray(hdrsize)
    hdr[0:3] = b"EXE"
    hdr[3] = 1
    struct.pack_into("<I", hdr, 4, hdrsize)        # OFFCOD = header size
    struct.pack_into("<H", hdr, 8, len(code))      # LOADER = loader size
    struct.pack_into("<H", hdr, 16, 0x8100)        # LD_ADDR
    struct.pack_into("<H", hdr, 18, 0x8100)        # PC_REG
    struct.pack_into("<H", hdr, 20, 0xBFFF)        # SP_REG

    with open(sys.argv[2], "wb") as f:
        f.write(hdr + code + blob)
    print("preexe: hdr=%d loader=%d blob=%d" % (hdrsize, len(code), len(blob)))


if __name__ == "__main__":
    main()
