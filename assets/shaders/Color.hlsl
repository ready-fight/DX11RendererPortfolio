cbuffer TransformConstants : register(b0)
{
    row_major float4x4 gWorld;
    row_major float4x4 gWorldViewProjection;
};

cbuffer MaterialConstants : register(b1)
{
    float4 gBaseColor;
};

cbuffer LightConstants : register(b2)
{
    float4 gLightDirection;
    float4 gLightColor;
    float4 gAmbientColor;
};


struct VSInput
{
    float3 position : POSITION;
    float3 color : COLOR;
    float3 normal : NORMAL;
};

struct VSOutput
{
    float4 position : SV_POSITION;
    float3 color : COLOR;
    float3 worldNormal : NORMAL;
};

VSOutput VSMain(VSInput input)
{
    VSOutput output;
    output.position = mul(float4(input.position, 1.0f), gWorldViewProjection);
    output.color = input.color;
    output.worldNormal = normalize(mul(float4(input.normal, 0.0f), gWorld).xyz);

    return output;
}


float4 PSMain(VSOutput input) : SV_TARGET
{
    float3 normal = normalize(input.worldNormal);
    float3 lightDirection = normalize(-gLightDirection.xyz);

    float diffuseAmount = saturate(dot(normal, lightDirection));

    float3 ambient = gAmbientColor.rgb;
    float3 diffuse = gLightColor.rgb * diffuseAmount;

    float3 finalColor = input.color * gBaseColor.rgb * (ambient + diffuse);

    return float4(finalColor, gBaseColor.a);
}