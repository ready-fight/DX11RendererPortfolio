#include "Engine/Renderer/DepthStencilBuffer.h"

#include "Engine/Core/Log.h"
#include "Engine/Renderer/DxHelpers.h"
#include "Engine/Renderer/GraphicsDevice.h"

namespace Engine
{
    bool DepthStencilBuffer::Initialize(GraphicsDevice& graphicsDevice, uint32_t width, uint32_t height)
    {
        D3D11_TEXTURE2D_DESC depthDesc = {};
        depthDesc.Width = width;
        depthDesc.Height = height;
        depthDesc.MipLevels = 1;
        depthDesc.ArraySize = 1;
        depthDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
        depthDesc.SampleDesc.Count = 1;
        depthDesc.SampleDesc.Quality = 0;
        depthDesc.Usage = D3D11_USAGE_DEFAULT;
        depthDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;

        HRESULT hr = graphicsDevice.GetDevice()->CreateTexture2D(
            &depthDesc,
            nullptr,
            m_buffer.GetAddressOf()
        );

        if (!DX_CHECK(hr))
        {
            return false;
        }

        hr = graphicsDevice.GetDevice()->CreateDepthStencilView(
            m_buffer.Get(),
            nullptr,
            m_depthStencilView.GetAddressOf()
        );

        if (!DX_CHECK(hr))
        {
            return false;
        }

        LogInfo("Depth stencil buffer initialized.");
        return true;
    }

    void DepthStencilBuffer::Shutdown()
    {
        m_depthStencilView.Reset();
        m_buffer.Reset();
    }
}