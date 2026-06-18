// src/Engine/Renderer/RenderTarget.h
#pragma once

#include <d3d11.h>
#include <wrl/client.h>

#include <cstdint>

namespace Engine {
  class GraphicsDevice;

  class RenderTarget final {
  public:
    bool InitializeFromBackBuffer(GraphicsDevice& graphicsDevice);

    bool InitializeColor(GraphicsDevice& graphicsDevice, uint32_t width, uint32_t height, DXGI_FORMAT format);

    void Shutdown();

    ID3D11RenderTargetView* GetRenderTargetView() const { return m_renderTargetView.Get(); }

    ID3D11ShaderResourceView* GetShaderResourceView() const { return m_shaderResourceView.Get(); }

    uint32_t GetWidth() const { return m_width; }
    uint32_t GetHeight() const { return m_height; }

  private:
    Microsoft::WRL::ComPtr<ID3D11Texture2D> m_texture;
    Microsoft::WRL::ComPtr<ID3D11RenderTargetView> m_renderTargetView;
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_shaderResourceView;

    uint32_t m_width = 0;
    uint32_t m_height = 0;
  };
}