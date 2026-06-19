cbuffer TransformConstants : register(b0)
{
    row_major float4x4 gWorld;
    row_major float4x4 gWorldViewProjection;
};

cbuffer MaterialConstants : register(b1)
{
    float4 gBaseColor;
    float4 gSpecular;
    float4 gEmissive;
};

struct VSInput
{
    float3 position : POSITION;
    float3 color : COLOR;
    float3 normal : NORMAL;
    float2 texcoord : TEXCOORD;
};

struct VSOutput
{
    float4 position : SV_POSITION;
    float3 normal : NORMAL;
};

VSOutput VSMain(VSInput input)
{
    VSOutput output;

    output.position = mul(float4(input.position, 1.0f), gWorldViewProjection);
    output.normal = mul(float4(input.normal, 0.0f), gWorld).xyz;

    return output;
}

float4 PSMain(VSOutput input) : SV_TARGET
{
    float3 normal = normalize(input.normal);

    // Convert normal range from -1..1 into visible color range 0..1.
    float3 normalColor = normal * 0.5f + 0.5f;

    return float4(normalColor * gBaseColor.rgb, 1.0f);
}