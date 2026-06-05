// src/Engine/Renderer/GpuBuffer.h
#pragma once

#include <d3d11.h>
#include <wrl/client.h>

#include <cstdint>

namespace Engine
{
    class GraphicsDevice;

    class GpuBuffer final
    {
    public:
        bool CreateVertexBuffer(
            GraphicsDevice& graphicsDevice,
            const void* data,
            uint32_t stride,
            uint32_t count
        );

        void Shutdown();
        void BindVertexBuffer(GraphicsDevice& graphicsDevice);

        uint32_t GetCount() const { return m_count; }

    private:
        Microsoft::WRL::ComPtr<ID3D11Buffer> m_buffer;

        uint32_t m_stride = 0;
        uint32_t m_count = 0;
    };
}