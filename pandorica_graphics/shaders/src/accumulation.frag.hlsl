#include "common.hlsli"

struct Output {
    float4 moments      : SV_Target0;
    float absorbance    : SV_Target1;
};

Texture2D tex       : register(t0, space2);
SamplerState smp    : register(s0, space2);

Output main(FS_Input input) {
    Output output;

    uint2 texSize;
    tex.GetDimensions(texSize.x, texSize.y);

    float4 texColor = SampleTexture(tex, smp, input.uv, input.uvMax);

    float depth = WarpDepth(input.pos.w);

    float alpha = texColor.a * input.color.a;

    if (alpha < 1e-5 || alpha > 1 - 1e-5) discard;

    output.absorbance = -log(clamp(1.0 - alpha, 0.001, 0.999));

    output.moments = float4(
        depth,
        depth * depth,
        depth * depth * depth,
        depth * depth * depth * depth
    ) * output.absorbance;

    return output;
}
