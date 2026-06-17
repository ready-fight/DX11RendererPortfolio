// src/Engine/Renderer/DepthStencilBuffer.h
#pragma once

#include <d3d11.h>
#include <wrl/client.h>

#include <cstdint>

namespace Engine {
  class GraphicsDevice;

  class DepthStencilBuffer final {
  public:
    bool Initialize(GraphicsDevice& graphicsDevice, uint32_t width, uint32_t height);
    void Shutdown();

    ID3D11DepthStencilView* GetDepthStencilView() const { return m_depthStencilView.Get(); }

    ID3D11ShaderResourceView* GetShaderResourceView() const { return m_shaderResourceView.Get(); }

  private:
    Microsoft::WRL::ComPtr<ID3D11Texture2D> m_buffer;
    Microsoft::WRL::ComPtr<ID3D11DepthStencilView> m_depthStencilView;
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_shaderResourceView;
  };
}