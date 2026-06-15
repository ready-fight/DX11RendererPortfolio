#pragma once

#include "Engine/Renderer/GpuBuffer.h"
#include "Engine/Renderer/MaterialInstance.h"
#include "Engine/Renderer/MaterialTypes.h"
#include "Engine/Renderer/ShaderProgram.h"

#include <d3d11.h>

namespace Engine {
  class GraphicsDevice;
  class RenderResourceManager;

  class Material final {
  public:
    bool Initialize(GraphicsDevice& graphicsDevice, const MaterialDesc& desc,
                    const D3D11_INPUT_ELEMENT_DESC* inputElements, unsigned int inputElementCount);

    void Shutdown();

    void Bind(GraphicsDevice& graphicsDevice, RenderResourceManager& renderResources,
              const MaterialInstance& materialInstance);

  private:
    ShaderProgram m_shader;
    GpuBuffer m_materialBuffer;

    MaterialShaderType m_shaderType = MaterialShaderType::LitTextured;
    TextureHandle m_baseTexture;
  };
}