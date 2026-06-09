#pragma once

#include "Engine/Scene/Transform.h"

namespace Engine
{
    class Mesh;

    struct SceneObject final
    {
        Mesh* mesh = nullptr;
        Transform transform;
        float rotationSpeed = 1.0f;
    };
}