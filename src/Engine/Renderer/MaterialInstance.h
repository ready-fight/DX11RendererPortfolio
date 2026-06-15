#pragma once

#include "Engine/Renderer/RenderResourceHandles.h"

#include <DirectXMath.h>

namespace Engine
{
    struct MaterialInstance final
    {
        MaterialHandle material;
        DirectX::XMFLOAT4 baseColor = { 1.0f, 1.0f, 1.0f, 1.0f };
    };
}