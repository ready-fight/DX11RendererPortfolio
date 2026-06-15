#pragma once

#include "Engine/Renderer/MaterialInstance.h"
#include "Engine/Renderer/RenderResourceHandles.h"
#include "Engine/Scene/Transform.h"

#include <string>

namespace Engine {
  class Mesh;
  class Material;

  struct SceneObject final {

    std::string name;

    MeshHandle mesh;
    MaterialInstance materialInstance;

    Transform transform;

    float rotationSpeed = 1.0f;
    bool enabled = true;
  };
}