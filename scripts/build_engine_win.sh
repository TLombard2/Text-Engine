#!/usr/bin/env bash
# ──────────────────────────────────────────────────────────────
#  Cross-compile the ENGINE for Windows (from WSL / Linux)
#
#  Prerequisites (run once):
#    sudo apt install gcc-mingw-w64-x86-64
#    Then place Windows SDL2 dev libs in win_deps/ — see README
# ──────────────────────────────────────────────────────────────
set -euo pipefail
cd "$(dirname "$0")/.."

CC=x86_64-w64-mingw32-gcc
DEPS=win_deps/SDL2

if [ ! -d "$DEPS" ]; then
    echo "ERROR: $DEPS not found."
    echo "Download SDL2, SDL2_ttf, and SDL2_image mingw dev packages"
    echo "and extract them into win_deps/SDL2/  (see scripts/setup_win_deps.sh)"
    exit 1
fi

mkdir -p build/win

$CC engine/src/*.c \
    -Iengine/include \
    -I"$DEPS/include" \
    -L"$DEPS/lib" \
    -lmingw32 -lSDL2main -lSDL2 -lSDL2_ttf -lSDL2_image \
    -mwindows \
    -o build/win/engine_main.exe

# Copy SDL2 DLLs and assets alongside the exe
cp "$DEPS/lib/"*.dll build/win/ 2>/dev/null || true
cp -r assets build/win/assets 2>/dev/null || true

echo "✓ Engine built: build/win/engine_main.exe"
echo "  Ready to run — DLLs and assets are in build/win/"
