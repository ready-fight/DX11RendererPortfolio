#pragma once

#include "Engine/Renderer/RenderResourceHandles.h"
#include "Engine/Renderer/ShaderProgram.h"

#include <d3d11.h>

namespace Engine {
  class GraphicsDevice;
  class RenderResourceManager;

  class Material final {
  public:
    bool Initialize(GraphicsDevice& graphicsDevice, const wchar_t* shaderPath,
                    const D3D11_INPUT_ELEMENT_DESC* inputElements, unsigned int inputElementCount);

    void Shutdown();

    void SetBaseTexture(TextureHandle textureHandle) { m_baseTexture = textureHandle; }

    void Bind(GraphicsDevice& graphicsDevice, RenderResourceManager& renderResources);

  private:
    ShaderProgram m_shader;
    TextureHandle m_baseTexture;
  };
}