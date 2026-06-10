#include "Engine/Renderer/RenderTarget.h"

#include "Engine/Core/Log.h"
#include "Engine/Renderer/DxHelpers.h"
#include "Engine/Renderer/GraphicsDevice.h"

namespace Engine
{
    bool RenderTarget::InitializeFromBackBuffer(GraphicsDevice& graphicsDevice)
    {
        HRESULT hr = graphicsDevice.GetSwapChain()->GetBuffer(
            0,
            __uuidof(ID3D11Texture2D),
            reinterpret_cast<void**>(m_texture.GetAddressOf())
        );

        if (!DX_CHECK(hr))
        {
            return false;
        }

        hr = graphicsDevice.GetDevice()->CreateRenderTargetView(
            m_texture.Get(),
            nullptr,
            m_renderTargetView.GetAddressOf()
        );

        if (!DX_CHECK(hr))
        {
            return false;
        }

        LogInfo("Back buffer render target initialized.");
        return true;
    }

    bool RenderTarget::InitializeColor(
        GraphicsDevice& graphicsDevice,
        uint32_t width,
        uint32_t height,
        DXGI_FORMAT format)
    {
        D3D11_TEXTURE2D_DESC textureDesc = {};
        textureDesc.Width = width;
        textureDesc.Height = height;
        textureDesc.MipLevels = 1;
        textureDesc.ArraySize = 1;
        textureDesc.Format = format;
        textureDesc.SampleDesc.Count = 1;
        textureDesc.SampleDesc.Quality = 0;
        textureDesc.Usage = D3D11_USAGE_DEFAULT;
        textureDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;

        HRESULT hr = graphicsDevice.GetDevice()->CreateTexture2D(
            &textureDesc,
            nullptr,
            m_texture.GetAddressOf()
        );

        if (!DX_CHECK(hr))
        {
            return false;
        }

        hr = graphicsDevice.GetDevice()->CreateRenderTargetView(
            m_texture.Get(),
            nullptr,
            m_renderTargetView.GetAddressOf()
        );

        if (!DX_CHECK(hr))
        {
            return false;
        }

        hr = graphicsDevice.GetDevice()->CreateShaderResourceView(
            m_texture.Get(),
            nullptr,
            m_shaderResourceView.GetAddressOf()
        );

        if (!DX_CHECK(hr))
        {
            return false;
        }

        LogInfo("Color render target initialized.");
        return true;
    }

    void RenderTarget::Shutdown()
    {
        m_shaderResourceView.Reset();
        m_renderTargetView.Reset();
        m_texture.Reset();
    }
}