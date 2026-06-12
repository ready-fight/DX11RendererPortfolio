#pragma once

#include "Engine/Renderer/RenderResourceHandles.h"
#include "Engine/Scene/Transform.h"

#include <DirectXMath.h>
#include <string>

namespace Engine {
  class Mesh;
  class Material;

  struct SceneObject final {

    std::string name;

    MeshHandle mesh;
    MaterialHandle material;

    Transform transform;

    DirectX::XMFLOAT4 baseColor = {1.0f, 1.0f, 1.0f, 1.0f};

    float rotationSpeed = 1.0f;
    bool enabled = true;
  };
}