// src/Engine/Renderer/GraphicsDevice.h
#pragma once

#include <d3d11.h>
#include <dxgi.h>
#include <wrl/client.h>

#include <cstdint>

namespace Engine
{
    class GraphicsDevice final
    {
    public:
        bool Initialize(void* nativeWindowHandle, uint32_t width, uint32_t height);
        void Shutdown();

        void BeginFrame(float r, float g, float b, float a);
        void EndFrame();

    private:
        Microsoft::WRL::ComPtr<ID3D11Device> m_device;
        Microsoft::WRL::ComPtr<ID3D11DeviceContext> m_context;
        Microsoft::WRL::ComPtr<IDXGISwapChain> m_swapChain;
        Microsoft::WRL::ComPtr<ID3D11RenderTargetView> m_backBufferRTV;
    };
}