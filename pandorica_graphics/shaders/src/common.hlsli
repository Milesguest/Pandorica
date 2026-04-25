struct VS_Input {
    // Vertex
    float3 position     : TEXCOORD0;
    float2 uv           : TEXCOORD1;

    // Instance
    float3 instPosition : TEXCOORD2;
    float2 instSize     : TEXCOORD3;
    float3 instOrigin   : TEXCOORD4;
    float3 instRotX     : TEXCOORD5;
    float3 instRotY     : TEXCOORD6;
    float3 instRotZ     : TEXCOORD7;
    uint instColor      : TEXCOORD8;
    float2 instCutPos   : TEXCOORD9;
    float2 instCutSize  : TEXCOORD10;
};

struct FS_Input {
    float4 pos      : SV_Position;
    float2 uv       : TEXCOORD0;
    nointerpolation
    float4 color    : TEXCOORD1;
    nointerpolation
    float4 uvMax    : TEXCOORD2;
};

static const float AMBIENCE = 0.02;

static const float BIAS = 0.005;

// Camera near/far
static const float Z_MIN = 0.001;
static const float Z_MAX = 1000.0;

float WarpDepth(float linearZ) {
    linearZ = linearZ;

    linearZ = max(linearZ, Z_MIN);

    float logMin = log(Z_MIN);

    return saturate((log(linearZ) - logMin) / (log(Z_MAX) - logMin));
}

float4 SampleTexture(Texture2D tex, SamplerState smp, float2 uv, float4 uvMax) {
    float2 size;
    tex.GetDimensions(size.x, size.y);
    float2 invSize = 1.0 / size;

    float2 pixelCoords = uv * size;
    float2 width = fwidth(pixelCoords);
    float2 boxSize = clamp(width, 1e-5, 1.0);

    float2 tx = pixelCoords - 0.5;
    float2 fracTx = frac(tx);

    float2 blendWeights = smoothstep(0.5 - boxSize * 0.5, 0.5 + boxSize * 0.5, fracTx);

    float2 p00 = floor(tx) + 0.5; // Bottom-left texel center
    float2 p10 = (p00 + float2(1, 0)) * invSize;   // Bottom-right
    float2 p01 = (p00 + float2(0, 1)) * invSize;   // Top-left
    float2 p11 = (p00 + float2(1, 1)) * invSize;   // Top-right

    p00 *= invSize;

    float2 dx = ddx(uv);
    float2 dy = ddy(uv);

    float4 c00 = tex.SampleGrad(smp, p00, dx, dy);
    float4 c10 = tex.SampleGrad(smp, p10, dx, dy);
    float4 c01 = tex.SampleGrad(smp, p01, dx, dy);
    float4 c11 = tex.SampleGrad(smp, p11, dx, dy);

    if (p00.x < uvMax.x || p00.x > uvMax.z || p00.y < uvMax.y || p00.y > uvMax.w) c00.a = 0.0f;
    if (p01.x < uvMax.x || p01.x > uvMax.z || p01.y < uvMax.y || p01.y > uvMax.w) c01.a = 0.0f;
    if (p10.x < uvMax.x || p10.x > uvMax.z || p10.y < uvMax.y || p10.y > uvMax.w) c10.a = 0.0f;
    if (p11.x < uvMax.x || p11.x > uvMax.z || p11.y < uvMax.y || p11.y > uvMax.w) c11.a = 0.0f;

    c00.rgb *= c00.a;
    c10.rgb *= c10.a;
    c01.rgb *= c01.a;
    c11.rgb *= c11.a;

    float4 bottom = lerp(c00, c10, blendWeights.x);
    float4 top    = lerp(c01, c11, blendWeights.x);
    float4 result = lerp(bottom, top, blendWeights.y);

    if (result.a > 1e-5) {
        result.rgb /= result.a;
    }

    return result;
};
