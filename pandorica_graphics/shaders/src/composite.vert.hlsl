#include "common.hlsli"

FS_Input main(VS_Input input) {
    FS_Input output;
    output.pos = float4(input.position.xy * 2.0 - 1.0, 0.0, 1.0);
    output.uv = float2(input.position.x, 1 - input.position.y);
    output.color = float4(1.0,1.0,1.0,1.0);

    return output;
}
