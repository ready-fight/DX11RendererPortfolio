#pragma once

#include "Engine/Renderer/GpuBuffer.h"
#include "Engine/Renderer/PostProcessSettings.h"
#include "Engine/Renderer/ShaderProgram.h"


#include <d3d11.h>
#include <wrl/client.h>

namespace Engine {
  class GraphicsDevice;
  class RenderTarget;

  class FullscreenPass final {
  public:
    bool Initialize(GraphicsDevice& graphicsDevice);
    void Shutdown();

    void Render(GraphicsDevice& graphicsDevice, RenderTarget& sourceTexture, const PostProcessSettings& settings);

  private:
    ShaderProgram m_shader;
    GpuBuffer m_postProcessBuffer;
    Microsoft::WRL::ComPtr<ID3D11SamplerState> m_samplerState;
  };
}