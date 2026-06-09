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

        DirectX::XMFLOAT4 baseColor = { 1.0f, 1.0f, 1.0f, 1.0f };
        
        float rotationSpeed = 1.0f;
    };
}