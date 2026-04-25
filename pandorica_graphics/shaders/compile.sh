cd "$(dirname "$0")/src"

for filename in *.hlsl; do
    if [ -f "$filename" ]; then
        shadercross -g "$filename" -o "../spv/${filename/.hlsl/.spv}" -I "./"
        shadercross -g "$filename" -o "../msl/${filename/.hlsl/.msl}" -I "./"
        shadercross -g "$filename" -o "../dxil/${filename/.hlsl/.dxil}" -I "./"
    fi
done
