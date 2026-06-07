#pragma once

#include <d3d11.h>
#include <dxgi.h>
#include <wrl/client.h>

#include <cstdint>

namespace Engine {
  class GraphicsDevice final {
  public:
    bool Initialize(void* nativeWindowHandle, uint32_t width, uint32_t height);
    void Shutdown();

    void BeginFrame(float r, float g, float b, float a);
    void EndFrame();

    ID3D11Device* GetDevice() const { return m_device.Get(); }
    ID3D11DeviceContext* GetContext() const { return m_context.Get(); }

  private:
    void ReportLiveObjects();

  private:
    Microsoft::WRL::ComPtr<ID3D11Device> m_device;
    Microsoft::WRL::ComPtr<ID3D11DeviceContext> m_context;
    Microsoft::WRL::ComPtr<IDXGISwapChain> m_swapChain;
    Microsoft::WRL::ComPtr<ID3D11RenderTargetView> m_backBufferRTV;
    Microsoft::WRL::ComPtr<ID3D11Texture2D> m_depthStencilBuffer;
    Microsoft::WRL::ComPtr<ID3D11DepthStencilView> m_depthStencilView;

    bool m_debugLayerEnabled = false;
  };
}