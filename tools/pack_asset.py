#!/usr/bin/env python3
"""Pack SDK resource files into 16K HRUST chunks."""

from __future__ import annotations

import argparse
import os
import subprocess
import sys
import tempfile
from pathlib import Path


MAGIC = b"SPK1"
VERSION = 1
CHUNK_SIZE = 0x4000
FLAG_RAW = 0
FLAG_HRUST = 1


def default_mhmt() -> Path:
    env = os.environ.get("MHMT")
    if env:
        return Path(env)
    return Path(__file__).resolve().parents[2] / "kode" / "Tools" / "mhmt"


def run_mhmt(mhmt: Path, src: Path, dst: Path) -> None:
    subprocess.run(
        [str(mhmt), "-hst", "-zxh", str(src), str(dst)],
        check=True,
        stdout=subprocess.PIPE,
        stderr=subprocess.STDOUT,
    )


def verify_hst(mhmt: Path, packed: Path, expected: bytes, tmpdir: Path, index: int) -> None:
    depacked_path = tmpdir / f"chunk{index:03d}.dpk"
    subprocess.run(
        [str(mhmt), "-d", "-hst", "-zxh", str(packed), str(depacked_path)],
        check=True,
        stdout=subprocess.PIPE,
        stderr=subprocess.STDOUT,
    )
    actual = depacked_path.read_bytes()
    if actual != expected:
        raise ValueError(f"mhmt verification failed for chunk {index}")


def pack_chunk(mhmt: Path, chunk: bytes, index: int, tmpdir: Path) -> tuple[int, bytes]:
    raw_path = tmpdir / f"chunk{index:03d}.raw"
    hst_path = tmpdir / f"chunk{index:03d}.hst"
    raw_path.write_bytes(chunk)
    run_mhmt(mhmt, raw_path, hst_path)
    packed = hst_path.read_bytes()
    if len(packed) < len(chunk) and len(packed) <= CHUNK_SIZE:
        verify_hst(mhmt, hst_path, chunk, tmpdir, index)
        return FLAG_HRUST, packed
    return FLAG_RAW, chunk


def pack_file(mhmt: Path, src: Path, dst: Path) -> None:
    data = src.read_bytes()
    chunks = [data[i : i + CHUNK_SIZE] for i in range(0, len(data), CHUNK_SIZE)]
    if not chunks:
        raise ValueError("empty resources are not supported")
    if len(chunks) > 255:
        raise ValueError("resource is too large: more than 255 pages")

    dst.parent.mkdir(parents=True, exist_ok=True)
    with tempfile.TemporaryDirectory(prefix="sprpack-") as tmp:
        tmpdir = Path(tmp)
        with dst.open("wb") as out:
            out.write(MAGIC)
            out.write(bytes([VERSION, len(chunks), 0, 0]))
            packed_count = 0
            saved = 0
            for index, chunk in enumerate(chunks):
                flag, payload = pack_chunk(mhmt, chunk, index, tmpdir)
                if flag == FLAG_HRUST:
                    packed_count += 1
                    saved += len(chunk) - len(payload)
                out.write(bytes([flag]))
                out.write(len(chunk).to_bytes(2, "little"))
                out.write(len(payload).to_bytes(2, "little"))
                out.write(payload)

    print(
        f"{src.name}: {len(data)} -> {dst.stat().st_size} bytes, "
        f"{packed_count}/{len(chunks)} chunks packed, saved {saved} bytes"
    )


def main(argv: list[str]) -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--mhmt", type=Path, default=default_mhmt())
    parser.add_argument("input", type=Path)
    parser.add_argument("output", type=Path)
    args = parser.parse_args(argv)

    if not args.mhmt.is_file() or not os.access(args.mhmt, os.X_OK):
        parser.error(f"mhmt tool is not executable: {args.mhmt}")

    pack_file(args.mhmt, args.input, args.output)
    return 0


if __name__ == "__main__":
    raise SystemExit(main(sys.argv[1:]))
