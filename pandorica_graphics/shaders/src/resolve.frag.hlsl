#include "common.hlsli"

struct Input {
    float4 pos      : SV_Position;
    float2 uv       : TEXCOORD0;
    float4 color    : TEXCOORD1;
};

Texture2D mainTex   : register(t0, space2);
SamplerState smp    : register(s0, space2);

// From accumulation.frag
Texture2D momentTex         : register(t1, space2);
Texture2D absorbanceTex     : register(t2, space2);

// Reverse Z
#define IS_INVERTED_DEPTH 1

float ReconstructTransmittance(float4 moments, float absorbance, float z_f) {
    if (absorbance <= 1e-5) return 1.0;

    // Normalize moments
    float4 b = moments / absorbance;

    // Cholesky Decomposition of Hankel Matrix
    float l11 = 1.0;
    float l21 = b.x;
    float l22 = sqrt(max(1e-7, b.y - l21 * l21));
    float l31 = b.y;
    float l32 = (b.z - l31 * l21) / l22;
    float l33 = sqrt(max(1e-7, b.w - l31 * l31 - l32 * l32));

    // Solve L * y = Z(z_f)
    float z0 = z_f;
    float y1 = 1.0;
    float y2 = (z0 - l21) / l22;
    float y3 = (z0 * z0 - l31 - l32 * y2) / l33;

    // Solve L^T * q = y
    float q2 = y3 / l33;
    float q1 = (y2 - l32 * q2) / l22;
    float q0 = (y1 - l21 * q1 - l31 * q2);

    // Solve Quadratic q2*z^2 + q1*z + q0 = 0
    float2 roots;
    float discriminant = q1 * q1 - 4.0 * q2 * q0;

    if (abs(q2) > 1e-6 && discriminant >= 0.0) {
        float sqrtD = sqrt(discriminant);
        roots = float2(
            (-q1 - sqrtD) / (2.0 * q2),
            (-q1 + sqrtD) / (2.0 * q2)
        );
    } else {
        // Degenerate case:
        // We have one valid root. The second must be pushed "behind" everything
        // so it doesn't count as a blocker.
        float validRoot = -q0 / (sign(q1) * max(abs(q1), 1e-7));

        #if IS_INVERTED_DEPTH
            roots = float2(validRoot, -99999.0); // -Inf is "Behind" in Reverse Z
        #else
            roots = float2(validRoot, 99999.0);  // +Inf is "Behind" in Standard
        #endif
    }

    // Determine Weights
    // We only sum up roots that are in front of our query depth.
    float v1, v2;

    #if IS_INVERTED_DEPTH
        // Reverse Z (1=Near, 0=Far): "In Front" means value > z0
        v1 = (roots.x > z0) ? 1.0 : 0.0;
        v2 = (roots.y > z0) ? 1.0 : 0.0;
    #else
        // Standard (0=Near, 1=Far): "In Front" means value < z0
        v1 = (roots.x < z0) ? 1.0 : 0.0;
        v2 = (roots.y < z0) ? 1.0 : 0.0;
    #endif

    // Lagrange Basis Integration
    float3 z = float3(z0, roots.x, roots.y);
    float3 v = float3(0.0, v1, v2); // v0=0 (

    float d0 = (z.x - z.y) * (z.x - z.z);
    float d1 = (z.y - z.x) * (z.y - z.z);
    float d2 = (z.z - z.x) * (z.z - z.y);

    // Prevent division by zero if roots match query depth exactly
    d0 = sign(d0) * max(abs(d0), 1e-7);
    d1 = sign(d1) * max(abs(d1), 1e-7);
    d2 = sign(d2) * max(abs(d2), 1e-7);

    float E_0 = (b.y - b.x * (z.y + z.z) + (z.y * z.z)) / d0;
    float E_1 = (b.y - b.x * (z.x + z.z) + (z.x * z.z)) / d1;
    float E_2 = (b.y - b.x * (z.x + z.y) + (z.x * z.y)) / d2;

    float fractionalAbsorbance = v.x * E_0 + v.y * E_1 + v.z * E_2;

    return exp(-absorbance * saturate(fractionalAbsorbance));
}

float4 main(FS_Input input) : SV_Target {
    uint2 texSize;
    mainTex.GetDimensions(texSize.x, texSize.y);

    float4 texColor = SampleTexture(mainTex, smp, input.uv, input.uvMax) * input.color;

    if (texColor.a < 1e-5 || texColor.a > 1 - 1e-5) discard;

    float4 moments = momentTex.Load(int3(input.pos.xy, 0.0));
    float absorbance = absorbanceTex.Load(int3(input.pos.xy, 0.0)).r;

    float depth = WarpDepth(input.pos.w);
    float queryDepth;

    #if IS_INVERTED_DEPTH
        queryDepth = depth + BIAS;
    #else
        queryDepth = depth - BIAS;
    #endif

    float transmittance = ReconstructTransmittance(moments, absorbance, queryDepth);

    return float4(texColor.rgb * texColor.a * transmittance, texColor.a * transmittance);
}
