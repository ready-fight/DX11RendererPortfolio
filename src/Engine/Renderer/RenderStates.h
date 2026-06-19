#pragma once

#include <d3d11.h>
#include <wrl/client.h>

namespace Engine {
  class GraphicsDevice;

  enum class BlendMode { Opaque, Alpha, Additive };

  class RenderStates final {
  public:
    bool Initialize(GraphicsDevice& graphicsDevice);
    void Shutdown();

    void Apply(GraphicsDevice& graphicsDevice, bool wireframeEnabled, BlendMode blendMode = BlendMode::Opaque);

  private:
    Microsoft::WRL::ComPtr<ID3D11RasterizerState> m_solidRasterizerState;
    Microsoft::WRL::ComPtr<ID3D11RasterizerState> m_wireframeRasterizerState;

    Microsoft::WRL::ComPtr<ID3D11DepthStencilState> m_depthEnabledState;

    Microsoft::WRL::ComPtr<ID3D11BlendState> m_opaqueBlendState;
    Microsoft::WRL::ComPtr<ID3D11BlendState> m_alphaBlendState;
    Microsoft::WRL::ComPtr<ID3D11BlendState> m_additiveBlendState;
  };
}