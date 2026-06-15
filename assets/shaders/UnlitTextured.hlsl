cbuffer TransformConstants : register(b0)
{
    row_major float4x4 gWorld;
    row_major float4x4 gWorldViewProjection;
};

cbuffer MaterialConstants : register(b1)
{
    float4 gBaseColor;
};

Texture2D gBaseTexture : register(t0);
SamplerState gBaseSampler : register(s0);

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
    float3 color : COLOR;
    float2 texcoord : TEXCOORD;
};

VSOutput VSMain(VSInput input)
{
    VSOutput output;
    output.position = mul(float4(input.position, 1.0f), gWorldViewProjection);
    output.color = input.color;
    output.texcoord = input.texcoord;
    return output;
}

float4 PSMain(VSOutput input) : SV_TARGET
{
    float4 textureColor = gBaseTexture.Sample(gBaseSampler, input.texcoord);
    float3 finalColor = input.color * gBaseColor.rgb * textureColor.rgb;

    return float4(finalColor, gBaseColor.a * textureColor.a);
}