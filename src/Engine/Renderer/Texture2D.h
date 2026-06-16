#pragma once

#include <d3d11.h>
#include <wrl/client.h>

#include <cstdint>
#include <string>

namespace Engine {
  class GraphicsDevice;

  class Texture2D final {
  public:
    bool LoadFromFile(GraphicsDevice& graphicsDevice, const wchar_t* filePath);
    bool CreateCheckerboard(GraphicsDevice& graphicsDevice, uint32_t width, uint32_t height);

    void Shutdown();

    void BindPS(GraphicsDevice& graphicsDevice, uint32_t textureSlot, uint32_t samplerSlot);

    void SetDebugName(const std::string& debugName) { m_debugName = debugName; }

    const std::string& GetDebugName() const { return m_debugName; }

    uint32_t GetWidth() const { return m_width; }
    uint32_t GetHeight() const { return m_height; }

  private:
    Microsoft::WRL::ComPtr<ID3D11Texture2D> m_texture;
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_shaderResourceView;
    Microsoft::WRL::ComPtr<ID3D11SamplerState> m_samplerState;

    std::string m_debugName = "Unnamed Texture";
    uint32_t m_width = 0;
    uint32_t m_height = 0;
  };
}