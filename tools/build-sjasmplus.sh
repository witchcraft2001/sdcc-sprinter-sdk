#!/bin/bash
# build-sjasmplus.sh — Build sjasmplus assembler from source
#
# Builds sjasmplus from the local source tree or clones from GitHub.

set -e

SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"
SDK_DIR="$(dirname "$SCRIPT_DIR")"

# Check for local sjasmplus source
SJASMPLUS_SRC="${SJASMPLUS_SRC:-/Users/dmitry/dev/zx/sjasmplus}"

if [ ! -d "$SJASMPLUS_SRC" ]; then
    echo "sjasmplus source not found at $SJASMPLUS_SRC"
    echo "Cloning from GitHub..."
    SJASMPLUS_SRC="$SDK_DIR/build/sjasmplus-src"
    mkdir -p "$SDK_DIR/build"
    git clone --recursive https://github.com/z00m128/sjasmplus.git "$SJASMPLUS_SRC"
fi

echo "=== Building sjasmplus ==="
echo "Source: $SJASMPLUS_SRC"
echo ""

cd "$SJASMPLUS_SRC"

# Clean previous build
make clean 2>/dev/null || true

# Build
make -j$(sysctl -n hw.ncpu 2>/dev/null || nproc 2>/dev/null || echo 4)

# Check result
if [ -f sjasmplus ]; then
    echo ""
    echo "Build successful!"
    echo "Binary: $SJASMPLUS_SRC/sjasmplus"

    # Copy to SDK tools
    mkdir -p "$SDK_DIR/tools/bin"
    cp sjasmplus "$SDK_DIR/tools/bin/"
    echo "Copied to: $SDK_DIR/tools/bin/sjasmplus"
else
    echo "Build failed!"
    exit 1
fi

echo ""
echo "Verify with: $SDK_DIR/tools/bin/sjasmplus --version"
