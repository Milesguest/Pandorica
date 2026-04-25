#include "common.hlsli"

cbuffer VPB : register(b0, space1) {
    float4x4 viewProj;
};
FS_Input main(VS_Input input) {
    FS_Input output;

    float2 bias = (input.position.xy * 2.0f - 1.0f) * 0.03125f;

    output.uv = input.uv * input.instCutSize + input.instCutPos + bias;

    output.uvMax = float4(
        input.instCutPos.x,
        input.instCutPos.y,
        input.instCutPos.x + input.instCutSize.x,
        input.instCutPos.y + input.instCutSize.y
    );

    output.color = float4((input.instColor >> 0 & 0xFF), (input.instColor >> 8 & 0xFF), (input.instColor >> 16 & 0xFF), (input.instColor >> 24 & 0xFF));
    output.color /= float4(255.0f, 255.0f, 255.0f, 255.0f);

    float3x3 rotationMatrix = float3x3(input.instRotX, input.instRotY, input.instRotZ);

    float3 pos = input.position;

    pos.xy = (pos.xy - input.instOrigin.xy) * input.instSize;

    pos.xy += bias;

    pos = mul(pos, rotationMatrix);

    pos += input.instPosition;

    output.pos = mul(viewProj, float4(pos, 1.0f));

    return output;
}
