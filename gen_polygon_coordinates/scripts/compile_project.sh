#!/bin/bash
set -euo pipefail

if [ ! -f "main.c" ]; then
    echo "Error: script must be run in the project folder containing the 'main.c' file."
    exit 1
fi

name_project=$(basename "$(realpath .)")

mkdir -p build
# shellcheck disable=SC2046
clang -Iinclude $(pkg-config --cflags raylib) -c "main.c" -o "main.o"

# shellcheck disable=SC2046
clang main.o -o "$name_project" $(pkg-config --libs raylib) -lm -lpthread -ldl -lrt -lX11
