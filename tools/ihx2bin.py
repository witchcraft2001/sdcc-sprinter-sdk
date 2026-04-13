#!/usr/bin/env python3
"""
ihx2bin.py — Convert Intel HEX to raw binary for ZX Sprinter

Usage:
    ihx2bin.py input.ihx output.bin [--org 0xC000]

The binary starts from the lowest address in the HEX file (or --org if given)
and extends to the highest. Gaps are filled with 0xFF.
"""

import sys
import argparse


def parse_ihx(filename):
    """Parse Intel HEX, return dict {addr: byte}"""
    data = {}
    with open(filename, 'r') as f:
        for line in f:
            line = line.strip()
            if not line.startswith(':'):
                continue
            blen = int(line[1:3], 16)
            addr = int(line[3:7], 16)
            rtype = int(line[7:9], 16)
            if rtype == 1:  # EOF
                break
            if rtype != 0:  # only data records
                continue
            for i in range(blen):
                b = int(line[9 + i * 2:11 + i * 2], 16)
                data[addr + i] = b
    return data


def main():
    parser = argparse.ArgumentParser(description='Convert Intel HEX to raw binary')
    parser.add_argument('input', help='Input .ihx file')
    parser.add_argument('output', help='Output .bin file')
    parser.add_argument('--org', default=None, help='Origin address (default: auto from HEX)')
    parser.add_argument('--fill', type=lambda x: int(x, 0), default=0xFF,
                        help='Fill byte for gaps (default: 0xFF)')
    args = parser.parse_args()

    data = parse_ihx(args.input)
    if not data:
        print("Error: no data in HEX file", file=sys.stderr)
        sys.exit(1)

    min_addr = min(data.keys())
    max_addr = max(data.keys())

    if args.org is not None:
        org = int(args.org, 0)
    else:
        org = min_addr

    size = max_addr - org + 1
    binary = bytearray([args.fill] * size)

    for addr, byte in data.items():
        offset = addr - org
        if 0 <= offset < size:
            binary[offset] = byte

    with open(args.output, 'wb') as f:
        f.write(binary)

    print(f"Created {args.output}: {size} bytes (org: 0x{org:04X}, end: 0x{max_addr:04X})")


if __name__ == '__main__':
    main()
