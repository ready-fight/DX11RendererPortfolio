#include "Engine/Renderer/RenderTarget.h"

#include "Engine/Core/Log.h"
#include "Engine/Renderer/DxHelpers.h"
#include "Engine/Renderer/GraphicsDevice.h"

namespace Engine
{
    bool RenderTarget::InitializeFromBackBuffer(GraphicsDevice& graphicsDevice)
    {
        Microsoft::WRL::ComPtr<ID3D11Texture2D> backBuffer;

        HRESULT hr = graphicsDevice.GetSwapChain()->GetBuffer(
            0,
            __uuidof(ID3D11Texture2D),
            reinterpret_cast<void**>(backBuffer.GetAddressOf())
        );

        if (!DX_CHECK(hr))
        {
            return false;
        }

        hr = graphicsDevice.GetDevice()->CreateRenderTargetView(
            backBuffer.Get(),
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

    void RenderTarget::Shutdown()
    {
        m_renderTargetView.Reset();
    }
}