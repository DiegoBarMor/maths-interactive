#!/bin/bash
set -euo pipefail

sudo apt update
sudo apt install build-essential git clang pkg-config cmake -y
sudo apt install libasound2-dev libx11-dev libxrandr-dev \
    libxi-dev libgl1-mesa-dev libglu1-mesa-dev libxcursor-dev \
    libxinerama-dev libwayland-dev libxkbcommon-dev -y

cd ~
if [[ -d raylib ]]; then
    echo "Raylib directory found, skipping cloning"
else
    git clone https://github.com/raysan5/raylib.git
fi

cd ~
if [[ -d emsdk ]]; then
    echo "EMSDK directory found, skipping installation"
else
    git clone https://github.com/emscripten-core/emsdk.git
    cd emsdk
    ./emsdk install latest
    ./emsdk activate latest
fi

cd ~/emsdk
# shellcheck disable=SC1091
source ./emsdk_env.sh

cd ~/raylib
mkdir -p "wasm"

echo "Compiling raylib library (emcc)..."
cd "src"
emcc -c rcore.c -Os -Wall -DPLATFORM_WEB -DGRAPHICS_API_OPENGL_ES2
emcc -c rshapes.c -Os -Wall -DPLATFORM_WEB -DGRAPHICS_API_OPENGL_ES2
emcc -c rtextures.c -Os -Wall -DPLATFORM_WEB -DGRAPHICS_API_OPENGL_ES2
emcc -c rtext.c -Os -Wall -DPLATFORM_WEB -DGRAPHICS_API_OPENGL_ES2
emcc -c rmodels.c -Os -Wall -DPLATFORM_WEB -DGRAPHICS_API_OPENGL_ES2
emcc -c raudio.c -Os -Wall -DPLATFORM_WEB
emar rcs libraylib.a rcore.o rshapes.o rtextures.o rtext.o rmodels.o raudio.o

for name in libraylib.a rcore.o rshapes.o rtextures.o rtext.o rmodels.o raudio.o; do
    mv $name ../wasm/
done
