#include "Engine/Renderer/ParticlePass.h"
#include "Engine/Renderer/Camera.h"
#include "Engine/Renderer/GraphicsDevice.h"
#include "Engine/Scene/ParticleSystemSettings.h"

#include <algorithm>
#include <cmath>

namespace Engine {
  namespace {
    struct ParticleConstants {
      DirectX::XMFLOAT4X4 viewProjection;

      DirectX::XMFLOAT4 centerSize;
      DirectX::XMFLOAT4 color;

      DirectX::XMFLOAT4 cameraRight;
      DirectX::XMFLOAT4 cameraUp;
    };

    static_assert(sizeof(ParticleConstants) % 16 == 0);
  }

  bool ParticlePass::Initialize(GraphicsDevice& graphicsDevice) {
    if (!m_shader.Initialize(graphicsDevice, L"assets/shaders/AdditiveParticle.hlsl", nullptr, 0)) {
      return false;
    }

    if (!m_particleBuffer.CreateConstantBuffer(graphicsDevice, sizeof(ParticleConstants))) {
      return false;
    }

    return true;
  }

  void ParticlePass::Shutdown() {
    m_particleBuffer.Shutdown();
    m_shader.Shutdown();
  }

  void ParticlePass::Render(GraphicsDevice& graphicsDevice, const Camera& camera,
                            const ParticleSystemSettings& settings, float totalTime) {
    using namespace DirectX;

    const XMMATRIX view = camera.GetViewMatrix();
    const XMMATRIX projection = camera.GetProjectionMatrix();
    const XMMATRIX viewProjection = view * projection;

    XMFLOAT4X4 viewProjectionFloat = {};
    XMStoreFloat4x4(&viewProjectionFloat, viewProjection);

    const XMMATRIX inverseView = XMMatrixInverse(nullptr, view);

    XMFLOAT3 cameraRight = {};
    XMFLOAT3 cameraUp = {};

    XMStoreFloat3(&cameraRight, inverseView.r[0]);
    XMStoreFloat3(&cameraUp, inverseView.r[1]);

    ID3D11DeviceContext* context = graphicsDevice.GetContext();

    context->IASetInputLayout(nullptr);
    context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    ID3D11Buffer* nullVertexBuffers[] = {nullptr};
    UINT nullStrides[] = {0};
    UINT nullOffsets[] = {0};

    context->IASetVertexBuffers(0, 1, nullVertexBuffers, nullStrides, nullOffsets);

    context->IASetIndexBuffer(nullptr, DXGI_FORMAT_R32_UINT, 0);

    m_shader.Bind(graphicsDevice);

    const int particleCount = std::clamp(settings.particleCount, 0, 128);

    if (particleCount <= 0) {
      return;
    }

    for (int i = 0; i < particleCount; ++i) {
      const float normalizedIndex = static_cast<float>(i) / static_cast<float>(particleCount);

      const float angle = totalTime * settings.orbitSpeed + normalizedIndex * DirectX::XM_2PI;

      const float x = std::cos(angle) * settings.orbitRadius;
      const float z = std::sin(angle) * settings.orbitRadius;

      const float y =
          settings.verticalOffset + std::sin(totalTime * 2.0f + static_cast<float>(i)) * settings.verticalAmplitude;

      const float size =
          settings.baseSize + std::sin(totalTime * 3.0f + static_cast<float>(i)) * settings.sizePulseAmount;

      ParticleConstants constants = {};
      constants.viewProjection = viewProjectionFloat;

      constants.centerSize = {x, y, z, size};

      constants.color = settings.color;

      constants.cameraRight = {cameraRight.x, cameraRight.y, cameraRight.z, 0.0f};

      constants.cameraUp = {cameraUp.x, cameraUp.y, cameraUp.z, 0.0f};

      m_particleBuffer.Update(graphicsDevice, &constants, sizeof(constants));

      m_particleBuffer.BindConstantBufferVS(graphicsDevice, 0);
      m_particleBuffer.BindConstantBufferPS(graphicsDevice, 0);

      graphicsDevice.Draw(6, 0);
    }
  }
}