#include "Engine/Renderer/MeshPass.h"

#include "Engine/Renderer/GraphicsDevice.h"
#include "Engine/Renderer/Material.h"
#include "Engine/Renderer/Mesh.h"
#include "Engine/Renderer/RenderResourceManager.h"
#include "Engine/Scene/Scene.h"

#include <DirectXMath.h>
#include <cstddef>
#include <iterator>

#include <memory>

namespace Engine {
  namespace {
    struct TransformConstants {
      DirectX::XMFLOAT4X4 world;
      DirectX::XMFLOAT4X4 worldViewProjection;
    };

    struct MaterialConstants {
      DirectX::XMFLOAT4 baseColor;
    };

    struct LightConstants {
      DirectX::XMFLOAT4 lightDirection;
      DirectX::XMFLOAT4 lightColor;
      DirectX::XMFLOAT4 ambientColor;
    };

    static_assert(sizeof(TransformConstants) % 16 == 0);
    static_assert(sizeof(MaterialConstants) % 16 == 0);
    static_assert(sizeof(LightConstants) % 16 == 0);

  }

  bool MeshPass::Initialize(GraphicsDevice& graphicsDevice) {
    if (!m_checkerTexture.CreateCheckerboard(graphicsDevice, 64, 64)) {
      return false;
    }

    if (!m_transformBuffer.CreateConstantBuffer(graphicsDevice, sizeof(TransformConstants))) {
      return false;
    }

    if (!m_materialBuffer.CreateConstantBuffer(graphicsDevice, sizeof(MaterialConstants))) {
      return false;
    }

    if (!m_lightBuffer.CreateConstantBuffer(graphicsDevice, sizeof(LightConstants))) {
      return false;
    }

    return true;
  }

  void MeshPass::Shutdown() {
    m_lightBuffer.Shutdown();
    m_materialBuffer.Shutdown();
    m_transformBuffer.Shutdown();
    m_checkerTexture.Shutdown();
  }

  void MeshPass::Render(
    GraphicsDevice& graphicsDevice,
    RenderResourceManager& renderResources,
    const Camera& camera,
    Scene& scene,
    float totalSeconds) {
    using namespace DirectX;

    const XMMATRIX viewProjection = camera.GetViewProjectionMatrix();

    ID3D11DeviceContext* context = graphicsDevice.GetContext();

    m_transformBuffer.BindConstantBufferVS(graphicsDevice, 0);

    context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    LightConstants lightConstants = {};
    lightConstants.lightDirection = {-0.4f, -1.0f, 0.3f, 0.0f};
    lightConstants.lightColor = {0.9f, 0.9f, 0.85f, 1.0f};
    lightConstants.ambientColor = {0.18f, 0.18f, 0.22f, 1.0f};

    m_lightBuffer.Update(graphicsDevice, &lightConstants, sizeof(lightConstants));

    for (SceneObject& object : scene.GetObjects()) {

      Mesh* mesh = renderResources.ResolveMesh(object.mesh);
      Material* material = renderResources.ResolveMaterial(object.material);

      if (!object.enabled || !mesh || !material) {
        continue;
      }

      object.transform.rotationRadians.x = totalSeconds * object.rotationSpeed * 0.5f;
      object.transform.rotationRadians.y = totalSeconds * object.rotationSpeed;

      const XMMATRIX world = object.transform.GetWorldMatrix();
      const XMMATRIX worldViewProjection = world * viewProjection;

      TransformConstants constants = {};
      XMStoreFloat4x4(&constants.world, world);
      XMStoreFloat4x4(&constants.worldViewProjection, worldViewProjection);

      m_transformBuffer.Update(graphicsDevice, &constants, sizeof(constants));

      MaterialConstants materialConstants = {};
      materialConstants.baseColor = object.baseColor;

      m_materialBuffer.Update(graphicsDevice, &materialConstants, sizeof(materialConstants));

      material->Bind(graphicsDevice);
      m_transformBuffer.BindConstantBufferVS(graphicsDevice, 0);
      m_materialBuffer.BindConstantBufferPS(graphicsDevice, 1);
      m_lightBuffer.BindConstantBufferPS(graphicsDevice, 2);

      m_checkerTexture.BindPS(graphicsDevice, 0, 0);

      mesh->Bind(graphicsDevice);
      mesh->Draw(graphicsDevice);

      graphicsDevice.AddVisibleObject();
    }
  }
}