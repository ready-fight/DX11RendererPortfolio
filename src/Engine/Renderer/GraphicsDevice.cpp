// src/Engine/Renderer/GraphicsDevice.cpp
#include "Engine/Renderer/GraphicsDevice.h"

#include <Windows.h>
#include <stdexcept>

namespace Engine
{
    bool GraphicsDevice::Initialize(void* nativeWindowHandle, uint32_t width, uint32_t height)
    {
        HWND hwnd = static_cast<HWND>(nativeWindowHandle);

        DXGI_SWAP_CHAIN_DESC swapChainDesc = {};
        swapChainDesc.BufferCount = 1;
        swapChainDesc.BufferDesc.Width = width;
        swapChainDesc.BufferDesc.Height = height;
        swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
        swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
        swapChainDesc.OutputWindow = hwnd;
        swapChainDesc.SampleDesc.Count = 1;
        swapChainDesc.SampleDesc.Quality = 0;
        swapChainDesc.Windowed = TRUE;
        swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

        UINT createDeviceFlags = 0;

#if defined(_DEBUG)
        createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

        D3D_FEATURE_LEVEL featureLevels[] =
        {
            D3D_FEATURE_LEVEL_11_0
        };

        D3D_FEATURE_LEVEL createdFeatureLevel = {};

        HRESULT hr = D3D11CreateDeviceAndSwapChain(
            nullptr,
            D3D_DRIVER_TYPE_HARDWARE,
            nullptr,
            createDeviceFlags,
            featureLevels,
            1,
            D3D11_SDK_VERSION,
            &swapChainDesc,
            m_swapChain.GetAddressOf(),
            m_device.GetAddressOf(),
            &createdFeatureLevel,
            m_context.GetAddressOf()
        );

        if (FAILED(hr))
        {
            return false;
        }

        Microsoft::WRL::ComPtr<ID3D11Texture2D> backBuffer;

        hr = m_swapChain->GetBuffer(
            0,
            __uuidof(ID3D11Texture2D),
            reinterpret_cast<void**>(backBuffer.GetAddressOf())
        );

        if (FAILED(hr))
        {
            return false;
        }

        hr = m_device->CreateRenderTargetView(
            backBuffer.Get(),
            nullptr,
            m_backBufferRTV.GetAddressOf()
        );

        if (FAILED(hr))
        {
            return false;
        }

        D3D11_VIEWPORT viewport = {};
        viewport.TopLeftX = 0.0f;
        viewport.TopLeftY = 0.0f;
        viewport.Width = static_cast<float>(width);
        viewport.Height = static_cast<float>(height);
        viewport.MinDepth = 0.0f;
        viewport.MaxDepth = 1.0f;

        m_context->RSSetViewports(1, &viewport);

        return true;
    }

    void GraphicsDevice::Shutdown()
    {
        m_backBufferRTV.Reset();
        m_swapChain.Reset();
        m_context.Reset();
        m_device.Reset();
    }

    void GraphicsDevice::BeginFrame(float r, float g, float b, float a)
    {
        const float clearColor[] = { r, g, b, a };

        ID3D11RenderTargetView* renderTargets[] =
        {
            m_backBufferRTV.Get()
        };

        m_context->OMSetRenderTargets(1, renderTargets, nullptr);
        m_context->ClearRenderTargetView(m_backBufferRTV.Get(), clearColor);
    }

    void GraphicsDevice::EndFrame()
    {
        m_swapChain->Present(1, 0);
    }
}