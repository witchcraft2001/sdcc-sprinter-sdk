#!/bin/bash
# install-sdcc.sh — Install SDCC compiler for Z80 development
#
# Installs SDCC 4.1.0 or compatible version.
# Supports macOS (brew), Linux (apt/snap/source), Windows (MSYS2).

set -e

SDCC_VERSION="4.1.0"

echo "=== SDCC Installer for ZX Sprinter SDK ==="
echo "Target version: ${SDCC_VERSION}+"
echo ""

# Detect OS
case "$(uname -s)" in
    Darwin)
        echo "Platform: macOS"
        if command -v brew &>/dev/null; then
            echo "Installing SDCC via Homebrew..."
            brew install sdcc
            echo ""
            echo "Installed:"
            sdcc --version | head -1
        else
            echo "Homebrew not found. Install it from https://brew.sh"
            echo "Then run: brew install sdcc"
            exit 1
        fi
        ;;

    Linux)
        echo "Platform: Linux"
        if command -v apt-get &>/dev/null; then
            echo "Installing SDCC via apt..."
            sudo apt-get update
            sudo apt-get install -y sdcc
        elif command -v dnf &>/dev/null; then
            echo "Installing SDCC via dnf..."
            sudo dnf install -y sdcc
        elif command -v pacman &>/dev/null; then
            echo "Installing SDCC via pacman..."
            sudo pacman -S --noconfirm sdcc
        elif command -v snap &>/dev/null; then
            echo "Installing SDCC via snap..."
            sudo snap install sdcc
        else
            echo "No supported package manager found."
            echo "Download SDCC from: https://sdcc.sourceforge.net/"
            echo "Or build from source (see below)."
            echo ""
            echo "Build from source:"
            echo "  wget https://sourceforge.net/projects/sdcc/files/sdcc/${SDCC_VERSION}/sdcc-src-${SDCC_VERSION}.tar.bz2"
            echo "  tar xjf sdcc-src-${SDCC_VERSION}.tar.bz2"
            echo "  cd sdcc-${SDCC_VERSION}"
            echo "  ./configure --disable-pic14-port --disable-pic16-port"
            echo "  make -j\$(nproc)"
            echo "  sudo make install"
            exit 1
        fi
        echo ""
        echo "Installed:"
        sdcc --version | head -1
        ;;

    MINGW*|MSYS*|CYGWIN*)
        echo "Platform: Windows (MSYS2/Cygwin)"
        if command -v pacman &>/dev/null; then
            echo "Installing SDCC via MSYS2..."
            pacman -S --noconfirm mingw-w64-x86_64-sdcc
        else
            echo "Download SDCC from: https://sdcc.sourceforge.net/"
            echo "Windows installer: sdcc-${SDCC_VERSION}-x64-setup.exe"
            exit 1
        fi
        ;;

    *)
        echo "Unsupported platform: $(uname -s)"
        echo "Download SDCC from: https://sdcc.sourceforge.net/"
        exit 1
        ;;
esac

echo ""
echo "SDCC installation complete."
echo "Verify with: sdcc --version"
