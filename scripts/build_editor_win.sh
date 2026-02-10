#!/usr/bin/env bash
# ──────────────────────────────────────────────────────────────
#  Cross-compile the EDITOR for Windows (from WSL / Linux)
#
#  Prerequisites (run once):
#    sudo apt install gcc-mingw-w64-x86-64
#    Then place Windows raylib in win_deps/ — see README
# ──────────────────────────────────────────────────────────────
set -euo pipefail
cd "$(dirname "$0")/.."

CC=x86_64-w64-mingw32-gcc
DEPS=win_deps/raylib

if [ ! -d "$DEPS" ]; then
    echo "ERROR: $DEPS not found."
    echo "Download the raylib Windows (mingw-w64) release and"
    echo "extract it into win_deps/raylib/  (see scripts/setup_win_deps.sh)"
    exit 1
fi

mkdir -p build/win

$CC editor/src/*.c \
    -Ieditor/include \
    -I"$DEPS/include" \
    -L"$DEPS/lib" \
    -lraylib -lopengl32 -lgdi32 -lwinmm -lcomdlg32 \
    -mwindows \
    -o build/win/editor_main.exe

echo "✓ Editor built: build/win/editor_main.exe"
