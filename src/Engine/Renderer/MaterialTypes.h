#pragma once

#include "Engine/Renderer/RenderResourceHandles.h"

namespace Engine
{
    enum class MaterialShaderType
    {
        LitTextured,
        UnlitTextured
    };

    struct MaterialDesc final
    {
        MaterialShaderType shaderType = MaterialShaderType::LitTextured;
        TextureHandle baseTexture;
    };
}