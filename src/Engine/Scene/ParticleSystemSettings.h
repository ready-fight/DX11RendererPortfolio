#pragma once

#include <DirectXMath.h>

namespace Engine
{
    struct ParticleSystemSettings final
    {
        int particleCount = 12;

        float orbitRadius = 1.35f;
        float orbitSpeed = 1.2f;

        float verticalOffset = 0.15f;
        float verticalAmplitude = 0.35f;

        float baseSize = 0.10f;
        float sizePulseAmount = 0.025f;

        DirectX::XMFLOAT4 color =
        {
            0.25f,
            0.75f,
            1.0f,
            0.85f
        };
    };
}