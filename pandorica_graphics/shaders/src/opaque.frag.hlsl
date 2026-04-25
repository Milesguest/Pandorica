#include "common.hlsli"

Texture2D tex       : register(t0, space2);
SamplerState smp    : register(s0, space2);

float4 main(FS_Input input) : SV_Target {

    float4 color = SampleTexture(tex, smp, input.uv, input.uvMax) * input.color;

    if (color.a <= 1 - 1e-5) discard;

    return color;
}
