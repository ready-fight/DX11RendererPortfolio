#pragma once

#include "Engine/Renderer/RenderResourceHandles.h"
#include <string>

namespace Engine {
  enum class MaterialShaderType { LitTextured, UnlitTextured };

  inline const char* ToString(MaterialShaderType shaderType) {
    switch (shaderType) {
      case MaterialShaderType::LitTextured:
        return "Lit Textured";

      case MaterialShaderType::UnlitTextured:
        return "Unlit Textured";

      default:
        return "Unknown";
    }
  }

  struct MaterialDesc final {
    std::string debugName = "Unnamed Material";
    MaterialShaderType shaderType = MaterialShaderType::LitTextured;
    TextureHandle baseTexture;
  };
}