#!/usr/bin/env python3
"""
ihx2exe.py — Convert Intel HEX (.ihx) to ZX Sprinter DSS EXE format.

Usage: ihx2exe.py input.ihx output.exe [--entry 0x8100] [--stack 0xBFFF]

DSS EXE header format (512 bytes, from emulator SprintEXE struct):

  Offset  Size  Field
  0x00    3     id          "EXE" signature
  0x03    1     version     format version (0x01)
  0x04    4     offset      code offset in file (uint32 LE, usually 0x200 = 512)
  0x08    2     loader      loader size (0 = no loader)
  0x0A    2     loader2     reserved (0)
  0x0C    2     reserv1     reserved (0)
  0x0E    2     reserv2     reserved (0)
  0x10    2     load        load address in memory (e.g. 0x8100)
  0x12    2     start       entry point address (e.g. 0x8100)
  0x14    2     stack       initial SP (e.g. 0xBFFF)
  0x16    490   reserved    zeros
  0x200   ...   code        program code
"""

import sys
import struct
import argparse


def parse_ihx(filename):
    """Parse Intel HEX file, return dict of address -> byte."""
    data = {}
    with open(filename, 'r') as f:
        for line in f:
            line = line.strip()
            if not line or line[0] != ':':
                continue
            raw = bytes.fromhex(line[1:])
            length = raw[0]
            addr = (raw[1] << 8) | raw[2]
            rtype = raw[3]

            if rtype == 0:  # Data record
                for i in range(length):
                    data[addr + i] = raw[4 + i]
            elif rtype == 1:  # EOF
                break
    return data


def make_exe(data, load_addr=0x8100, entry=0x8100, stack=0xBFFF):
    """Create DSS EXE binary from parsed IHX data."""

    if not data:
        print("Error: no data in IHX file", file=sys.stderr)
        sys.exit(1)

    min_addr = min(data.keys())
    max_addr = max(data.keys())

    if min_addr < load_addr:
        print(f"Warning: data at 0x{min_addr:04X} is below load address 0x{load_addr:04X}",
              file=sys.stderr)

    # Build code binary
    code_size = max_addr - load_addr + 1
    code = bytearray(code_size)
    for addr, byte in data.items():
        offset = addr - load_addr
        if 0 <= offset < code_size:
            code[offset] = byte

    # Build 512-byte EXE header (matching SprintEXE struct)
    header = bytearray(512)

    # +0x00: "EXE" signature (3 bytes)
    header[0:3] = b'EXE'
    # +0x03: version (1 byte)
    header[3] = 0x01
    # +0x04: offset — file offset to code start (uint32 LE)
    struct.pack_into('<I', header, 0x04, 512)
    # +0x08: loader size (uint16 LE, 0 = no loader)
    struct.pack_into('<H', header, 0x08, 0)
    # +0x0A: loader2 / reserved
    struct.pack_into('<H', header, 0x0A, 0)
    # +0x0C: reserved
    struct.pack_into('<H', header, 0x0C, 0)
    # +0x0E: reserved
    struct.pack_into('<H', header, 0x0E, 0)
    # +0x10: load address (uint16 LE)
    struct.pack_into('<H', header, 0x10, load_addr)
    # +0x12: start/entry point address (uint16 LE)
    struct.pack_into('<H', header, 0x12, entry)
    # +0x14: stack pointer (uint16 LE)
    struct.pack_into('<H', header, 0x14, stack)

    return header + code


def main():
    parser = argparse.ArgumentParser(
        description='Convert Intel HEX to ZX Sprinter DSS EXE format')
    parser.add_argument('input', help='Input .ihx file')
    parser.add_argument('output', help='Output .exe file')
    parser.add_argument('--entry', type=lambda x: int(x, 0), default=0x8100,
                        help='Entry point address (default: 0x8100)')
    parser.add_argument('--stack', type=lambda x: int(x, 0), default=0xBFFF,
                        help='Initial stack pointer (default: 0xBFFF)')
    parser.add_argument('--load', type=lambda x: int(x, 0), default=0x8100,
                        help='Load address (default: 0x8100)')
    args = parser.parse_args()

    data = parse_ihx(args.input)
    exe = make_exe(data, load_addr=args.load, entry=args.entry, stack=args.stack)

    with open(args.output, 'wb') as f:
        f.write(exe)

    code_size = len(exe) - 512
    print(f"Created {args.output}: {len(exe)} bytes "
          f"(header: 512, code: {code_size})")
    print(f"  Load: 0x{args.load:04X}, Entry: 0x{args.entry:04X}, "
          f"Stack: 0x{args.stack:04X}")


if __name__ == '__main__':
    main()
