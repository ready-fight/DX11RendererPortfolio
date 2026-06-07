// src/Engine/Renderer/Camera.cpp
#include "Engine/Renderer/Camera.h"

namespace Engine
{
    void Camera::SetPosition(float x, float y, float z)
    {
        m_position = { x, y, z };
    }

    void Camera::SetTarget(float x, float y, float z)
    {
        m_target = { x, y, z };
    }

    void Camera::SetPerspective(
        float verticalFovRadians,
        float aspectRatio,
        float nearPlane,
        float farPlane)
    {
        m_verticalFovRadians = verticalFovRadians;
        m_aspectRatio = aspectRatio;
        m_nearPlane = nearPlane;
        m_farPlane = farPlane;
    }

    DirectX::XMMATRIX Camera::GetViewMatrix() const
    {
        const DirectX::XMVECTOR position = DirectX::XMLoadFloat3(&m_position);
        const DirectX::XMVECTOR target = DirectX::XMLoadFloat3(&m_target);
        const DirectX::XMVECTOR up = DirectX::XMLoadFloat3(&m_up);

        return DirectX::XMMatrixLookAtLH(position, target, up);
    }

    DirectX::XMMATRIX Camera::GetProjectionMatrix() const
    {
        return DirectX::XMMatrixPerspectiveFovLH(
            m_verticalFovRadians,
            m_aspectRatio,
            m_nearPlane,
            m_farPlane
        );
    }

    DirectX::XMMATRIX Camera::GetViewProjectionMatrix() const
    {
        return GetViewMatrix() * GetProjectionMatrix();
    }
}