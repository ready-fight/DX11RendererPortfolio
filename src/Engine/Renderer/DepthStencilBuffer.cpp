#include "Engine/Renderer/DepthStencilBuffer.h"

#include "Engine/Core/Log.h"
#include "Engine/Renderer/DxHelpers.h"
#include "Engine/Renderer/GraphicsDevice.h"

namespace Engine {
  bool DepthStencilBuffer::Initialize(GraphicsDevice& graphicsDevice, uint32_t width, uint32_t height) {
    D3D11_TEXTURE2D_DESC depthDesc = {};
    depthDesc.Width = width;
    depthDesc.Height = height;
    depthDesc.MipLevels = 1;
    depthDesc.ArraySize = 1;
    depthDesc.Format = DXGI_FORMAT_R24G8_TYPELESS;
    depthDesc.SampleDesc.Count = 1;
    depthDesc.SampleDesc.Quality = 0;
    depthDesc.Usage = D3D11_USAGE_DEFAULT;
    depthDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;

    HRESULT hr = graphicsDevice.GetDevice()->CreateTexture2D(&depthDesc, nullptr, m_buffer.GetAddressOf());

    if (!DX_CHECK(hr)) {
      return false;
    }

    D3D11_DEPTH_STENCIL_VIEW_DESC dsvDesc = {};
    dsvDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
    dsvDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
    dsvDesc.Texture2D.MipSlice = 0;

    hr =
        graphicsDevice.GetDevice()->CreateDepthStencilView(m_buffer.Get(), &dsvDesc, m_depthStencilView.GetAddressOf());

    if (!DX_CHECK(hr)) {
      return false;
    }

    D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
    srvDesc.Format = DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
    srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
    srvDesc.Texture2D.MostDetailedMip = 0;
    srvDesc.Texture2D.MipLevels = 1;

    hr = graphicsDevice.GetDevice()->CreateShaderResourceView(
        m_buffer.Get(), &srvDesc, m_shaderResourceView.GetAddressOf());

    if (!DX_CHECK(hr)) {
      return false;
    }

    LogInfo("Depth stencil buffer initialized.");
    return true;
  }

  void DepthStencilBuffer::Shutdown() {
    m_depthStencilView.Reset();
    m_buffer.Reset();
    m_shaderResourceView.Reset();
  }
}