#pragma once

#include "Engine/Renderer/GpuBuffer.h"
#include "Engine/Renderer/MaterialInstance.h"
#include "Engine/Renderer/MaterialTypes.h"
#include "Engine/Renderer/ShaderProgram.h"

#include <d3d11.h>
#include <string>

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

    const std::string& GetDebugName() const { return m_debugName; }
    MaterialShaderType GetShaderType() const { return m_shaderType; }

  private:
    ShaderProgram m_shader;
    GpuBuffer m_materialBuffer;

    std::string m_debugName;
    MaterialShaderType m_shaderType = MaterialShaderType::LitTextured;
    TextureHandle m_baseTexture;
  };
}