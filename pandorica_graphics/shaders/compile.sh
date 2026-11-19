#!/bin/bash

SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"

export LD_LIBRARY_PATH="$SCRIPT_DIR/dxc-libs:$LD_LIBRARY_PATH"

cd "$SCRIPT_DIR/src"

for filename in *.hlsl; do
    if [ -f "$filename" ]; then
        shadercross -g "$filename" -o "../spv/${filename/.hlsl/.spv}" -I "./"
        shadercross -g "$filename" -o "../msl/${filename/.hlsl/.msl}" -I "./"
        shadercross -g "$filename" -o "../dxil/${filename/.hlsl/.dxil}" -I "./"
    fi
done
