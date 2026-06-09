// src/Engine/Renderer/Camera.cpp
#include "Engine/Renderer/Camera.h"
#include <algorithm>
#include <cmath>

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

     void Camera::SetOrbit(float yawRadians, float pitchRadians, float distance)
    {
        m_orbitYawRadians = yawRadians;
        m_orbitPitchRadians = pitchRadians;
        m_orbitDistance = distance;

        RecalculateOrbitPosition();
    }

    void Camera::AddOrbit(float yawDeltaRadians, float pitchDeltaRadians)
    {
        m_orbitYawRadians += yawDeltaRadians;
        m_orbitPitchRadians += pitchDeltaRadians;

        RecalculateOrbitPosition();
    }

    void Camera::AddDistance(float distanceDelta)
    {
        m_orbitDistance += distanceDelta;

        RecalculateOrbitPosition();
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

    void Camera::RecalculateOrbitPosition()
    {
        constexpr float minPitch = -DirectX::XM_PIDIV2 + 0.05f;
        constexpr float maxPitch = DirectX::XM_PIDIV2 - 0.05f;

        m_orbitPitchRadians = std::clamp(m_orbitPitchRadians, minPitch, maxPitch);
        m_orbitDistance = std::clamp(m_orbitDistance, 1.5f, 20.0f);

        const float cosPitch = std::cos(m_orbitPitchRadians);
        const float sinPitch = std::sin(m_orbitPitchRadians);
        const float sinYaw = std::sin(m_orbitYawRadians);
        const float cosYaw = std::cos(m_orbitYawRadians);

        m_position.x = m_target.x + m_orbitDistance * cosPitch * sinYaw;
        m_position.y = m_target.y + m_orbitDistance * sinPitch;
        m_position.z = m_target.z - m_orbitDistance * cosPitch * cosYaw;
    }
}