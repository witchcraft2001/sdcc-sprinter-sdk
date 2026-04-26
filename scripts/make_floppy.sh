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

check_83() {
    local name base ext
    name="$(basename "$1")"
    base="${name%.*}"
    ext="${name##*.}"

    if [ "$base" = "$name" ]; then
        ext=""
    fi

    if [ ${#base} -gt 8 ] || [ ${#ext} -gt 3 ]; then
        echo "ERROR: '$name' is not DOS 8.3 compatible; DSS does not support long filenames." >&2
        exit 1
    fi
}

# Copy all example EXE files
for exe in "$SDK_DIR"/examples/*/*.exe; do
    [ -f "$exe" ] || continue
    check_83 "$exe"
    name=$(basename "$exe" | tr '[:lower:]' '[:upper:]')
    mcopy -i "$OUTPUT" "$exe" "::$name"
    echo "  $name"
done

# Copy example graphics resources
for res in "$SDK_DIR"/examples/*/*.gfx; do
    [ -f "$res" ] || continue
    check_83 "$res"
    name=$(basename "$res" | tr '[:lower:]' '[:upper:]')
    mcopy -i "$OUTPUT" "$res" "::$name"
    echo "  $name"
done

# Copy example music resources
for res in "$SDK_DIR"/examples/*/*.pt3; do
    [ -f "$res" ] || continue
    check_83 "$res"
    name=$(basename "$res" | tr '[:lower:]' '[:upper:]')
    mcopy -i "$OUTPUT" "$res" "::$name"
    echo "  $name"
done

echo ""
echo "Floppy image: $OUTPUT"
mdir -i "$OUTPUT" ::
