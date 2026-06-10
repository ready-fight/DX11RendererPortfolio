Texture2D gSceneTexture : register(t0);
SamplerState gSceneSampler : register(s0);

struct VSOutput {
  float4 position : SV_POSITION;
  float2 texcoord : TEXCOORD;
};

VSOutput VSMain(uint vertexId : SV_VertexID) {
  float2 positions[3] = {float2(-1.0f, -1.0f), float2(-1.0f, 3.0f),
                         float2(3.0f, -1.0f)};

  float2 texcoords[3] = {float2(0.0f, 1.0f), float2(0.0f, -1.0f),
                         float2(2.0f, 1.0f)};

  VSOutput output;
  output.position = float4(positions[vertexId], 0.0f, 1.0f);
  output.texcoord = texcoords[vertexId];
  return output;
};

float4 PSMain(VSOutput input) : SV_TARGET
{
    return gSceneTexture.Sample(gSceneSampler, input.texcoord);
}