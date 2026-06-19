cbuffer TransformConstants : register(b0) {
  row_major float4x4 gWorld;
  row_major float4x4 gWorldViewProjection;
};

cbuffer MaterialConstants : register(b1) {
  float4 gBaseColor;
  float4 gSpecular;
  float4 gEmissive;
};

cbuffer LightConstants : register(b2) {
  float4 gLightDirection;
  float4 gLightColor;
  float4 gAmbientColor;
  float4 gCameraPosition;
};

Texture2D gBaseTexture : register(t0);
SamplerState gBaseSampler : register(s0);

struct VSInput {
  float3 position : POSITION;
  float3 color : COLOR;
  float3 normal : NORMAL;
  float2 texcoord : TEXCOORD;
};

struct VSOutput {
  float4 position : SV_POSITION;
  float3 worldPosition : POSITION;
  float3 color : COLOR;
  float3 normal : NORMAL;
  float2 texcoord : TEXCOORD;
};

VSOutput VSMain(VSInput input) {
  VSOutput output;

  float4 worldPosition = mul(float4(input.position, 1.0f), gWorld);

  output.position = mul(float4(input.position, 1.0f), gWorldViewProjection);
  output.worldPosition = worldPosition.xyz;
  output.color = input.color;
  output.normal = mul(float4(input.normal, 0.0f), gWorld).xyz;
  output.texcoord = input.texcoord;

  return output;
}

float4 PSMain(VSOutput input) : SV_TARGET {
  float4 textureColor = gBaseTexture.Sample(gBaseSampler, input.texcoord);

  float3 baseColor = input.color * gBaseColor.rgb * textureColor.rgb;

  float3 normal = normalize(input.normal);

  float3 lightDir = normalize(-gLightDirection.xyz);
  float diffuseAmount = saturate(dot(normal, lightDir));

  float3 viewDir = normalize(gCameraPosition.xyz - input.worldPosition);
  float3 reflectDir = reflect(-lightDir, normal);

  float specularStrength = gSpecular.x;
  float specularPower = max(gSpecular.y, 1.0f);

  float specularAmount =
      pow(saturate(dot(viewDir, reflectDir)), specularPower) * specularStrength;

  float3 ambient = baseColor * gAmbientColor.rgb;
  float3 diffuse = baseColor * gLightColor.rgb * diffuseAmount;
  float3 specular = gLightColor.rgb * specularAmount;

  float3 finalColor = ambient + diffuse + specular + gEmissive.rgb;

  return float4(finalColor, gBaseColor.a * textureColor.a);
}