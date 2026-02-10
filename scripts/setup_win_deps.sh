#!/usr/bin/env bash
# ──────────────────────────────────────────────────────────────
#  Download & set up Windows cross-compile dependencies
#     • SDL2, SDL2_ttf, SDL2_image  (for the engine)
#     • raylib                      (for the editor)
#
#  Run this once from WSL.  Requires: wget, unzip, tar
# ──────────────────────────────────────────────────────────────
set -euo pipefail
cd "$(dirname "$0")/.."

TMPDIR=$(mktemp -d)
trap "rm -rf $TMPDIR" EXIT

mkdir -p win_deps/SDL2/{include/SDL2,lib}
mkdir -p win_deps/raylib/{include,lib}

echo "=== Installing mingw-w64 cross-compiler (if missing) ==="
if ! command -v x86_64-w64-mingw32-gcc &>/dev/null; then
    sudo apt-get update -qq
    sudo apt-get install -y gcc-mingw-w64-x86-64
fi

# ──────────────────────────────────────────────────────────────
#  SDL2 libraries (mingw development releases from libsdl.org)
# ──────────────────────────────────────────────────────────────
SDL2_VER="2.30.10"
SDL2_TTF_VER="2.22.0"
SDL2_IMG_VER="2.8.4"

echo "=== Downloading SDL2 $SDL2_VER ==="
wget -q "https://github.com/libsdl-org/SDL/releases/download/release-${SDL2_VER}/SDL2-devel-${SDL2_VER}-mingw.tar.gz" \
     -O "$TMPDIR/sdl2.tar.gz"
tar -xzf "$TMPDIR/sdl2.tar.gz" -C "$TMPDIR"
cp "$TMPDIR/SDL2-${SDL2_VER}/x86_64-w64-mingw32/include/SDL2/"* win_deps/SDL2/include/SDL2/
find "$TMPDIR/SDL2-${SDL2_VER}/x86_64-w64-mingw32/lib" -maxdepth 1 -type f -exec cp {} win_deps/SDL2/lib/ \;
cp "$TMPDIR/SDL2-${SDL2_VER}/x86_64-w64-mingw32/bin/"*.dll      win_deps/SDL2/lib/

echo "=== Downloading SDL2_ttf $SDL2_TTF_VER ==="
wget -q "https://github.com/libsdl-org/SDL_ttf/releases/download/release-${SDL2_TTF_VER}/SDL2_ttf-devel-${SDL2_TTF_VER}-mingw.tar.gz" \
     -O "$TMPDIR/sdl2_ttf.tar.gz"
tar -xzf "$TMPDIR/sdl2_ttf.tar.gz" -C "$TMPDIR"
cp "$TMPDIR/SDL2_ttf-${SDL2_TTF_VER}/x86_64-w64-mingw32/include/SDL2/"* win_deps/SDL2/include/SDL2/
find "$TMPDIR/SDL2_ttf-${SDL2_TTF_VER}/x86_64-w64-mingw32/lib" -maxdepth 1 -type f -exec cp {} win_deps/SDL2/lib/ \;
cp "$TMPDIR/SDL2_ttf-${SDL2_TTF_VER}/x86_64-w64-mingw32/bin/"*.dll      win_deps/SDL2/lib/

echo "=== Downloading SDL2_image $SDL2_IMG_VER ==="
wget -q "https://github.com/libsdl-org/SDL_image/releases/download/release-${SDL2_IMG_VER}/SDL2_image-devel-${SDL2_IMG_VER}-mingw.tar.gz" \
     -O "$TMPDIR/sdl2_img.tar.gz"
tar -xzf "$TMPDIR/sdl2_img.tar.gz" -C "$TMPDIR"
cp "$TMPDIR/SDL2_image-${SDL2_IMG_VER}/x86_64-w64-mingw32/include/SDL2/"* win_deps/SDL2/include/SDL2/
find "$TMPDIR/SDL2_image-${SDL2_IMG_VER}/x86_64-w64-mingw32/lib" -maxdepth 1 -type f -exec cp {} win_deps/SDL2/lib/ \;
cp "$TMPDIR/SDL2_image-${SDL2_IMG_VER}/x86_64-w64-mingw32/bin/"*.dll      win_deps/SDL2/lib/

# ──────────────────────────────────────────────────────────────
#  raylib  (pre-built mingw-w64 release)
# ──────────────────────────────────────────────────────────────
RAYLIB_VER="5.5"

echo "=== Downloading raylib $RAYLIB_VER ==="
wget -q "https://github.com/raysan5/raylib/releases/download/${RAYLIB_VER}/raylib-${RAYLIB_VER}_win64_mingw-w64.zip" \
     -O "$TMPDIR/raylib.zip"
unzip -qo "$TMPDIR/raylib.zip" -d "$TMPDIR"
cp "$TMPDIR/raylib-${RAYLIB_VER}_win64_mingw-w64/include/"* win_deps/raylib/include/
find "$TMPDIR/raylib-${RAYLIB_VER}_win64_mingw-w64/lib" -maxdepth 1 -type f -exec cp {} win_deps/raylib/lib/ \;

echo ""
echo "══════════════════════════════════════════════════════"
echo "  ✓ Windows dependencies installed into win_deps/"
echo ""
echo "  win_deps/SDL2/    – SDL2, SDL2_ttf, SDL2_image"
echo "  win_deps/raylib/  – raylib"
echo ""
echo "  Next steps:"
echo "    bash scripts/build_engine_win.sh"
echo "    bash scripts/build_editor_win.sh"
echo "══════════════════════════════════════════════════════"
