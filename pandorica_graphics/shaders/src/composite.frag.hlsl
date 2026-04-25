#include "common.hlsli"

Texture2D resolvedTex       : register(t0, space2);
Texture2D absorbanceTex     : register(t1, space2);
Texture2D backgroundTex     : register(t2, space2);

SamplerState smp            : register(s0, space2);

float4 main(FS_Input input) : SV_Target {
    float4 transparentColors = resolvedTex.Sample(smp, input.uv);

    float totalAbsorbance = absorbanceTex.Sample(smp, input.uv).r;
    float totalTransmittance = exp(-totalAbsorbance);

    float4 opaqueColors = backgroundTex.Sample(smp, input.uv);

    if (opaqueColors.a == 0.0f) {
        if (transparentColors.r == 0.0f && transparentColors.g == 0.0f && transparentColors.b == 0.0f)
            discard;
    } else {
        transparentColors.a = 1.0f;
    }

    return (float4(transparentColors.rgb + (opaqueColors.rgb * totalTransmittance), transparentColors.a));
}
