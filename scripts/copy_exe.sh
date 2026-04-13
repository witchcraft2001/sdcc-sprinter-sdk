#!/bin/bash
# copy_exe.sh — Copy all example EXE files to a target directory
#
# Usage: ./copy_exe.sh <destination_directory>

if [ -z "$1" ]; then
    echo "Usage: $0 <destination_directory>"
    echo "Example: $0 /path/to/emulator/disk/TEST"
    exit 1
fi

DEST="$1"
SDK_DIR="$(cd "$(dirname "$0")/.." && pwd)"

mkdir -p "$DEST"

for exe in "$SDK_DIR"/examples/*/*.exe; do
    [ -f "$exe" ] || continue
    name=$(basename "$exe" | tr '[:lower:]' '[:upper:]')
    cp -v "$exe" "$DEST/$name"
done

echo "Done. Files copied to $DEST"
