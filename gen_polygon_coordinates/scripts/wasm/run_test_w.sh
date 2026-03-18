#!/bin/bash
set -euo pipefail

if [ ! -f "main.c" ]; then
    echo "Error: script must be run in the project folder containing the 'main.c' file."
    exit 1
fi

PORT=8080
is_port_in_use() { # check if a local listener is already bound to the port
    if command -v ss >/dev/null 2>&1; then
        ss -ltn | grep -q ":$PORT" && return 0 || return 1
    elif command -v netstat >/dev/null 2>&1; then
        netstat -tln | grep -q ":$PORT" && return 0 || return 1
    elif command -v lsof >/dev/null 2>&1; then
        lsof -iTCP:"$PORT" -sTCP:LISTEN >/dev/null 2>&1 && return 0 || return 1
    else
        # fallback to bash /dev/tcp (requires bash builtin support)
        (echo > /dev/tcp/127.0.0.1/"$PORT") >/dev/null 2>&1 && return 0 || return 1
    fi
}

append_timestamp=false

name_project=$(basename "$(realpath .)")
folder_out=~/Desktop/gui_exec/$name_project

if [ ! -d "$folder_out" ]; then
    scripts/prepare_test.sh
fi

rm -rf "$folder_out/website"

cp "main.c" "$folder_out/main.c"
cp -r "website" "$folder_out/website"
if [ -d "assets" ]; then
    cp -r "assets" "$folder_out/assets"
fi

cd "$folder_out"
bash scripts/wasm/compile_project_w.sh
cd - >/dev/null

cd ~/emsdk
# shellcheck disable=SC1091
source ./emsdk_env.sh
cd - >/dev/null

if [ -z "${PREFIX+x}" ]; then # safe test for PREFIX even with "set -u"
    is_termux=""
else
    is_termux=$(printf '%s' "$PREFIX" | grep -o "com.termux" || true)
fi

ts=$(date "+%Y%m%d_%H%M%S")
if [ ! "$is_termux" ]; then
    if [ $append_timestamp == "true" ]; then
        echo "// $ts last compiled (other)" >> "src/main.c"
    fi

    ### run the executable
    cd "$folder_out"
    emrun --port $PORT "app.html"

elif [ $append_timestamp == "true" ]; then
    echo "// $ts last compiled (termux)" >> "src/main.c"
fi
