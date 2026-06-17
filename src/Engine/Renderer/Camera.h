#pragma once

#include <DirectXMath.h>

namespace Engine {
  class Camera final {
  public:
    void SetPosition(float x, float y, float z);
    void SetTarget(float x, float y, float z);

    void SetPerspective(float verticalFovRadians, float aspectRatio, float nearPlane, float farPlane);

    void SetOrbit(float yawRadians, float pitchRadians, float distance);
    void AddOrbit(float yawDeltaRadians, float pitchDeltaRadians);
    void AddDistance(float distanceDelta);

    DirectX::XMMATRIX GetViewMatrix() const;
    DirectX::XMMATRIX GetProjectionMatrix() const;
    DirectX::XMMATRIX GetViewProjectionMatrix() const;

    const DirectX::XMFLOAT3& GetPosition() const { return m_position; }

  private:
    void RecalculateOrbitPosition();

  private:
    DirectX::XMFLOAT3 m_position = {0.0f, 0.0f, -2.5f};
    DirectX::XMFLOAT3 m_target = {0.0f, 0.0f, 0.0f};
    DirectX::XMFLOAT3 m_up = {0.0f, 1.0f, 0.0f};

    float m_verticalFovRadians = DirectX::XMConvertToRadians(60.0f);
    float m_aspectRatio = 16.0f / 9.0f;
    float m_nearPlane = 0.1f;
    float m_farPlane = 100.0f;

    float m_orbitYawRadians = 0.0f;
    float m_orbitPitchRadians = 0.0f;
    float m_orbitDistance = 4.5f;
  };
}