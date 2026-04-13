#!/bin/bash
# make_floppy.sh — Create a FAT12 floppy image with example EXE files
#
# Requires: mtools (brew install mtools / apt install mtools)
#
# Usage: ./make_floppy.sh [output.img]

SDK_DIR="$(cd "$(dirname "$0")/.." && pwd)"
OUTPUT="${1:-$SDK_DIR/build/sprinter.img}"

mkdir -p "$(dirname "$OUTPUT")"

# Create 1.44MB floppy image (blank, filled with 0xF6)
dd if=/dev/zero of="$OUTPUT" bs=512 count=2880 2>/dev/null

# Format as FAT12 with mformat
# Drive letter 'a:' mapped to the image file
export MTOOLS_NO_VFAT=1
mformat -f 1440 -v SPRINTER -i "$OUTPUT" ::

# Copy all example EXE files
for exe in "$SDK_DIR"/examples/*/*.exe; do
    [ -f "$exe" ] || continue
    name=$(basename "$exe" | tr '[:lower:]' '[:upper:]')
    mcopy -i "$OUTPUT" "$exe" "::$name"
    echo "  $name"
done

echo ""
echo "Floppy image: $OUTPUT"
mdir -i "$OUTPUT" ::
