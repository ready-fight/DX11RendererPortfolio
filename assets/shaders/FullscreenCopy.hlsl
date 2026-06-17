// assets/shaders/FullscreenCopy.hlsl

cbuffer PostProcessConstants : register(b0)
{
    float4 gPostSettings;
    float4 gDebugSettings;
};

Texture2D gSceneTexture : register(t0);
Texture2D gSceneDepthTexture : register(t1);
SamplerState gSceneSampler : register(s0);

struct VSOutput
{
    float4 position : SV_POSITION;
    float2 texcoord : TEXCOORD;
};

VSOutput VSMain(uint vertexId : SV_VertexID)
{
    VSOutput output;

    float2 positions[3] =
    {
        float2(-1.0f, -1.0f),
        float2(-1.0f,  3.0f),
        float2( 3.0f, -1.0f)
    };

    float2 texcoords[3] =
    {
        float2(0.0f, 1.0f),
        float2(0.0f, -1.0f),
        float2(2.0f, 1.0f)
    };

    output.position = float4(positions[vertexId], 0.0f, 1.0f);
    output.texcoord = texcoords[vertexId];

    return output;
}

float LinearizeDepth(float depth)
{
    float nearPlane = gDebugSettings.y;
    float farPlane = gDebugSettings.z;

    return (nearPlane * farPlane) /
        max(farPlane - depth * (farPlane - nearPlane), 0.0001f);
}

float4 PSMain(VSOutput input) : SV_TARGET
{
    float4 color = gSceneTexture.Sample(gSceneSampler, input.texcoord);

    float grayscaleAmount = gPostSettings.x;
    float exposure = gPostSettings.y;
    float contrast = gPostSettings.z;
    float vignetteAmount = gPostSettings.w;

    float3 finalColor = color.rgb;

    finalColor *= exposure;

    finalColor = ((finalColor - 0.5f) * contrast) + 0.5f;

    float grayscale =
        dot(finalColor, float3(0.299f, 0.587f, 0.114f));

    finalColor =
        lerp(finalColor, grayscale.xxx, grayscaleAmount);

    float2 centeredUv = input.texcoord - 0.5f;
    float distanceFromCenter = length(centeredUv);

    float vignette =
        smoothstep(0.8f, 0.25f, distanceFromCenter);

    finalColor =
        lerp(finalColor, finalColor * vignette, vignetteAmount);

    float rawDepth =
        gSceneDepthTexture.Sample(gSceneSampler, input.texcoord).r;

    float linearDepth = LinearizeDepth(rawDepth);

    float depthRange = max(gDebugSettings.w, 0.001f);

    float normalizedDepth =
        saturate((linearDepth - gDebugSettings.y) / depthRange);

    float depthDisplay = 1.0f - normalizedDepth;

    float3 depthColor = depthDisplay.xxx;

    float depthVisualizationAmount = gDebugSettings.x;

    finalColor =
        lerp(finalColor, depthColor, depthVisualizationAmount);

    return float4(finalColor, color.a);
}