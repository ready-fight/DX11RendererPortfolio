#pragma once

#include "Engine/Renderer/GpuBuffer.h"
#include "Engine/Renderer/MaterialInstance.h"
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

    void Bind(GraphicsDevice& graphicsDevice, RenderResourceManager& renderResources, const MaterialInstance& materialInstance);

  private:
    ShaderProgram m_shader;
    GpuBuffer m_materialBuffer;
    TextureHandle m_baseTexture;
  };
}