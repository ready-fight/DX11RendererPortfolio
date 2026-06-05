#pragma once

#include <d3d11.h>
#include <wrl/client.h>

namespace Engine {
  class GraphicsDevice;

  class ShaderProgram final {
  public:
    bool Initialize(GraphicsDevice &graphicsDevice, const wchar_t *shaderPath,
                    const D3D11_INPUT_ELEMENT_DESC *inputElements, unsigned int inputElementCount);

    void Shutdown();
    void Bind(GraphicsDevice &graphicsDevice);

  private:
    Microsoft::WRL::ComPtr<ID3D11VertexShader> m_vertexShader;
    Microsoft::WRL::ComPtr<ID3D11PixelShader> m_pixelShader;
    Microsoft::WRL::ComPtr<ID3D11InputLayout> m_inputLayout;
  };
}