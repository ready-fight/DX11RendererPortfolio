#pragma once

#include "Engine/Renderer/RenderResourceHandles.h"

#include <DirectXMath.h>

namespace Engine {
  struct MaterialInstance final {
    MaterialHandle material;
    DirectX::XMFLOAT4 baseColor = {1.0f, 1.0f, 1.0f, 1.0f};

    float specularStrength = 0.35f;
    float specularPower = 32.0f;

    DirectX::XMFLOAT3 emissiveColor = {1.0f, 1.0f, 1.0f};
    float emissiveStrength = 0.0f;
  };
}