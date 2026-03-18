#!/bin/bash
set -euo pipefail

if [ ! -f "main.c" ]; then
    echo "Error: script must be run in the project folder containing the 'main.c' file."
    exit 1
fi

safe_copy_files() {
    ext=$1
    shopt -s nullglob
    files=(website/*."$ext")
    if [ ${#files[@]} -gt 0 ]; then
        cp "${files[@]}" ./
    fi
    shopt -u nullglob
}

NAME_SHELL="chocomint_io"

cd ~/emsdk
# shellcheck disable=SC1091
source ./emsdk_env.sh
cd - >/dev/null

path_libraylib=~/raylib/wasm/libraylib.a
path_raylib_h=~/raylib/src
path_shell="website/shell/$NAME_SHELL.html"

# rm -f ./*.html ./*.css ./*.js
safe_copy_files html
safe_copy_files css
safe_copy_files js

arg_preload=""
if [ -d "assets" ]; then
    arg_preload="--preload-file assets"
fi

emcc -Iinclude -I. -I"$path_raylib_h" -c "main.c" -o "main.o" \
    "$(pkg-config --cflags raylib 2>/dev/null || true)" -DPLATFORM_WEB

### Link object files into final HTML/WASM output
# shellcheck disable=SC2086
emcc main.o "$path_libraylib" -o "app.html" -Os -Wall \
    -I. -I"$path_raylib_h" -L"$(dirname "$path_libraylib")" \
    "$(pkg-config --cflags --libs raylib 2>/dev/null || true)" \
    -s USE_GLFW=3 --shell-file "$path_shell" -DPLATFORM_WEB \
    $arg_preload -s TOTAL_MEMORY=67108864 \
    -s EXPORTED_RUNTIME_METHODS=ccall
