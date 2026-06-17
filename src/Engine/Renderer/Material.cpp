#include "Engine/Renderer/Material.h"

#include "Engine/Core/Log.h"
#include "Engine/Renderer/GraphicsDevice.h"
#include "Engine/Renderer/RenderResourceManager.h"
#include "Engine/Renderer/Texture2D.h"

namespace Engine {

  namespace {

    const wchar_t* GetShaderPath(MaterialShaderType shaderType) {
      switch (shaderType) {
        case MaterialShaderType::LitTextured:
          return L"assets/shaders/Color.hlsl";

        case MaterialShaderType::UnlitTextured:
          return L"assets/shaders/UnlitTextured.hlsl";

        case MaterialShaderType::NormalVisualizer:
          return L"assets/shaders/NormalVisualizer.hlsl";

        default:
          return L"assets/shaders/Color.hlsl";
      }
    }

    struct MaterialConstants {
      DirectX::XMFLOAT4 baseColor;
      DirectX::XMFLOAT4 specular;
    };

    static_assert(sizeof(MaterialConstants) % 16 == 0);
  }

  bool Material::Initialize(GraphicsDevice& graphicsDevice, const MaterialDesc& desc,
                            const D3D11_INPUT_ELEMENT_DESC* inputElements, unsigned int inputElementCount) {

    m_debugName = desc.debugName;
    m_shaderType = desc.shaderType;
    m_baseTexture = desc.baseTexture;

    if (!m_shader.Initialize(graphicsDevice, GetShaderPath(desc.shaderType), inputElements, inputElementCount)) {
      return false;
    }

    if (!m_materialBuffer.CreateConstantBuffer(graphicsDevice, sizeof(MaterialConstants))) {
      return false;
    }

    LogInfo("Material initialized.");
    return true;
  }

  void Material::Shutdown() { m_shader.Shutdown(); }

  void Material::Bind(GraphicsDevice& graphicsDevice, RenderResourceManager& renderResources,
                      const MaterialInstance& materialInstance) {
    MaterialConstants constants = {};
    constants.baseColor = materialInstance.baseColor;
    constants.specular = {materialInstance.specularStrength, materialInstance.specularPower, 0.0f, 0.0f};

    m_materialBuffer.Update(graphicsDevice, &constants, sizeof(constants));

    m_shader.Bind(graphicsDevice);

    m_materialBuffer.BindConstantBufferPS(graphicsDevice, 1);

    Texture2D* texture = renderResources.ResolveTexture(m_baseTexture);

    if (texture) {
      texture->BindPS(graphicsDevice, 0, 0);
    }
  }
}