#pragma once

#include <DirectXMath.h>

namespace Engine
{
    struct DirectionalLight final
    {
        DirectX::XMFLOAT3 direction = { -0.4f, -1.0f, 0.2f };
        float intensity = 1.0f;

        DirectX::XMFLOAT3 color = { 1.0f, 1.0f, 1.0f };
        float ambientIntensity = 0.2f;

        DirectX::XMFLOAT3 ambientColor = { 1.0f, 1.0f, 1.0f };
    };
}