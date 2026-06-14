#pragma once

#include <d3d11.h>
#include <wrl/client.h>

#include <cstdint>

namespace Engine
{
    class GraphicsDevice;

    class Texture2D final
    {
    public:
    bool LoadFromFile(GraphicsDevice& graphicsDevice, const wchar_t* filePath);
        bool CreateCheckerboard(GraphicsDevice& graphicsDevice, uint32_t width, uint32_t height);
        
        void Shutdown();

        void BindPS(GraphicsDevice& graphicsDevice, uint32_t textureSlot, uint32_t samplerSlot);

    private:
        Microsoft::WRL::ComPtr<ID3D11Texture2D> m_texture;
        Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_shaderResourceView;
        Microsoft::WRL::ComPtr<ID3D11SamplerState> m_samplerState;
    };
}