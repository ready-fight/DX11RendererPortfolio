cbuffer ParticleConstants : register(b0)
{
    row_major float4x4 gViewProjection;

    float4 gCenterSize;
    float4 gColor;

    float4 gCameraRight;
    float4 gCameraUp;
};

struct VSOutput
{
    float4 position : SV_POSITION;
    float2 texcoord : TEXCOORD;
    float4 color : COLOR;
};

VSOutput VSMain(uint vertexId : SV_VertexID)
{
    float2 corners[6] =
        {
            float2(-1.0f, -1.0f),
            float2(-1.0f, 1.0f),
            float2(1.0f, 1.0f),

            float2(-1.0f, -1.0f),
            float2(1.0f, 1.0f),
            float2(1.0f, -1.0f)};

    float2 corner = corners[vertexId];

    float3 center = gCenterSize.xyz;
    float size = gCenterSize.w;

    float3 worldPosition =
        center +
        gCameraRight.xyz * corner.x * size +
        gCameraUp.xyz * corner.y * size;

    VSOutput output;
    output.position = mul(float4(worldPosition, 1.0f), gViewProjection);
    output.texcoord = corner * 0.5f + 0.5f;
    output.color = gColor;

    return output;
}

float4 PSMain(VSOutput input) : SV_TARGET
{
    float2 centeredUv = input.texcoord - 0.5f;
    float distanceFromCenter = length(centeredUv) * 2.0f;

    float glow = 1.0f - smoothstep(0.0f, 1.0f, distanceFromCenter);
    glow *= glow;

    float3 color = input.color.rgb * glow;
    float alpha = input.color.a * glow;

    return float4(color, alpha);
}