#include "Engine/Renderer/MeshPass.h"

#include "Engine/Renderer/GraphicsDevice.h"
#include "Engine/Renderer/MeshFactory.h"
#include "Engine/Renderer/VertexTypes.h"

#include <DirectXMath.h>
#include <cstddef>
#include <iterator>

namespace Engine {
  namespace {
    struct TransformConstants {
      DirectX::XMFLOAT4X4 worldViewProjection;
    };

    struct MaterialConstants {
      DirectX::XMFLOAT4 baseColor;
    };

    static_assert(sizeof(TransformConstants) % 16 == 0);
    static_assert(sizeof(MaterialConstants) % 16 == 0);
  }

  bool MeshPass::Initialize(GraphicsDevice& graphicsDevice) {
    D3D11_INPUT_ELEMENT_DESC inputElements[] = {{"POSITION",
                                                 0,
                                                 DXGI_FORMAT_R32G32B32_FLOAT,
                                                 0,
                                                 static_cast<UINT>(offsetof(VertexPositionColor, position)),
                                                 D3D11_INPUT_PER_VERTEX_DATA,
                                                 0},
                                                {"COLOR",
                                                 0,
                                                 DXGI_FORMAT_R32G32B32_FLOAT,
                                                 0,
                                                 static_cast<UINT>(offsetof(VertexPositionColor, color)),
                                                 D3D11_INPUT_PER_VERTEX_DATA,
                                                 0}};

    if (!m_colorMaterial.Initialize(graphicsDevice,
                                    L"assets/shaders/Color.hlsl",
                                    inputElements,
                                    static_cast<unsigned int>(std::size(inputElements)))) {
      return false;
    }

    if (!MeshFactory::CreateCube(graphicsDevice, m_cubeMesh)) {
      return false;
    }

    if (!m_transformBuffer.CreateConstantBuffer(graphicsDevice, sizeof(TransformConstants))) {
      return false;
    }

    if (!m_materialBuffer.CreateConstantBuffer(graphicsDevice, sizeof(MaterialConstants))) {
      return false;
    }

    SceneObject leftCube = {};
    leftCube.mesh = &m_cubeMesh;
    leftCube.material = &m_colorMaterial;
    leftCube.transform.position = {-1.5f, 0.0f, 0.0f};
    leftCube.transform.scale = {0.75f, 0.75f, 0.75f};
    leftCube.baseColor = {1.0f, 0.35f, 0.35f, 1.0f};
    leftCube.rotationSpeed = 0.75f;
    m_sceneObjects.push_back(leftCube);

    SceneObject centerCube = {};
    centerCube.mesh = &m_cubeMesh;
    centerCube.material = &m_colorMaterial;
    centerCube.transform.position = {0.0f, 0.0f, 0.0f};
    centerCube.transform.scale = {1.0f, 1.0f, 1.0f};
    centerCube.baseColor = {0.35f, 1.0f, 0.35f, 1.0f};
    centerCube.rotationSpeed = 1.25f;
    m_sceneObjects.push_back(centerCube);

    SceneObject rightCube = {};
    rightCube.mesh = &m_cubeMesh;
    rightCube.material = &m_colorMaterial;
    rightCube.transform.position = {1.5f, 0.0f, 0.0f};
    rightCube.transform.scale = {0.5f, 0.5f, 0.5f};
    rightCube.baseColor = {0.35f, 0.55f, 1.0f, 1.0f};
    rightCube.rotationSpeed = 2.0f;
    m_sceneObjects.push_back(rightCube);

    return true;
  }

  void MeshPass::Shutdown() {

    m_sceneObjects.clear();

    m_materialBuffer.Shutdown();
    m_transformBuffer.Shutdown();
    m_cubeMesh.Shutdown();
    m_colorMaterial.Shutdown();
  }

  void MeshPass::Render(GraphicsDevice& graphicsDevice, const Camera& camera, float totalSeconds) {
    using namespace DirectX;

    const XMMATRIX viewProjection = camera.GetViewProjectionMatrix();

    ID3D11DeviceContext* context = graphicsDevice.GetContext();

    m_transformBuffer.BindConstantBufferVS(graphicsDevice, 0);

    context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    for (SceneObject& object : m_sceneObjects) {
      if (!object.mesh || !object.material) {
        continue;
      }

      object.transform.rotationRadians.x = totalSeconds * object.rotationSpeed * 0.5f;
      object.transform.rotationRadians.y = totalSeconds * object.rotationSpeed;

      const XMMATRIX world = object.transform.GetWorldMatrix();
      const XMMATRIX worldViewProjection = world * viewProjection;

      TransformConstants constants = {};
      XMStoreFloat4x4(&constants.worldViewProjection, worldViewProjection);

      m_transformBuffer.Update(graphicsDevice, &constants, sizeof(constants));

      MaterialConstants materialConstants = {};
      materialConstants.baseColor = object.baseColor;

      m_materialBuffer.Update(graphicsDevice, &materialConstants, sizeof(materialConstants));

      object.material->Bind(graphicsDevice);
      m_transformBuffer.BindConstantBufferVS(graphicsDevice, 0);
      m_materialBuffer.BindConstantBufferPS(graphicsDevice, 1);

      object.mesh->Bind(graphicsDevice);
      object.mesh->Draw(graphicsDevice);
    }
  }
}