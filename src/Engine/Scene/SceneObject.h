#pragma once

#include "Engine/Scene/Transform.h"

namespace Engine
{
    class Mesh;
    class Material;

    struct SceneObject final
    {
        Mesh* mesh = nullptr;
        Material* material = nullptr;

        Transform transform;
        
        float rotationSpeed = 1.0f;
    };
}