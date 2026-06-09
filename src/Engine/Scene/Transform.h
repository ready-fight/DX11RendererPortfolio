#pragma once

#include <DirectXMath.h>

namespace Engine
{
    struct Transform final
    {
        DirectX::XMFLOAT3 position = { 0.0f, 0.0f, 0.0f };
        DirectX::XMFLOAT3 rotationRadians = { 0.0f, 0.0f, 0.0f };
        DirectX::XMFLOAT3 scale = { 1.0f, 1.0f, 1.0f };

        DirectX::XMMATRIX GetWorldMatrix() const
        {
            const DirectX::XMMATRIX scaleMatrix =
                DirectX::XMMatrixScaling(scale.x, scale.y, scale.z);

            const DirectX::XMMATRIX rotationMatrix =
                DirectX::XMMatrixRotationRollPitchYaw(
                    rotationRadians.x,
                    rotationRadians.y,
                    rotationRadians.z
                );

            const DirectX::XMMATRIX translationMatrix =
                DirectX::XMMatrixTranslation(position.x, position.y, position.z);

            return scaleMatrix * rotationMatrix * translationMatrix;
        }
    };
}