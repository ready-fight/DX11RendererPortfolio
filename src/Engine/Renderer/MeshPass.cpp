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

    static_assert(sizeof(TransformConstants) % 16 == 0);
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

    if (!m_shader.Initialize(graphicsDevice,
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

    SceneObject leftCube = {};
    leftCube.mesh = &m_cubeMesh;
    leftCube.transform.position = {-1.5f, 0.0f, 0.0f};
    leftCube.transform.scale = {0.75f, 0.75f, 0.75f};
    leftCube.rotationSpeed = 0.75f;
    m_sceneObjects.push_back(leftCube);

    SceneObject centerCube = {};
    centerCube.mesh = &m_cubeMesh;
    centerCube.transform.position = {0.0f, 0.0f, 0.0f};
    centerCube.transform.scale = {1.0f, 1.0f, 1.0f};
    centerCube.rotationSpeed = 1.25f;
    m_sceneObjects.push_back(centerCube);

    SceneObject rightCube = {};
    rightCube.mesh = &m_cubeMesh;
    rightCube.transform.position = {1.5f, 0.0f, 0.0f};
    rightCube.transform.scale = {0.5f, 0.5f, 0.5f};
    rightCube.rotationSpeed = 2.0f;
    m_sceneObjects.push_back(rightCube);

    return true;
  }

  void MeshPass::Shutdown() {

    m_sceneObjects.clear();

    m_transformBuffer.Shutdown();
    m_cubeMesh.Shutdown();
    m_shader.Shutdown();
  }

  void MeshPass::Render(GraphicsDevice& graphicsDevice, const Camera& camera, float totalSeconds) {
    using namespace DirectX;

    const XMMATRIX viewProjection = camera.GetViewProjectionMatrix();

    ID3D11DeviceContext* context = graphicsDevice.GetContext();

    m_shader.Bind(graphicsDevice);
    m_transformBuffer.BindConstantBufferVS(graphicsDevice, 0);

    context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    for (SceneObject& object : m_sceneObjects) {
      if (!object.mesh) {
        continue;
      }

      object.transform.rotationRadians.x = totalSeconds * object.rotationSpeed * 0.5f;
      object.transform.rotationRadians.y = totalSeconds * object.rotationSpeed;

      const XMMATRIX world = object.transform.GetWorldMatrix();
      const XMMATRIX worldViewProjection = world * viewProjection;

      TransformConstants constants = {};
      XMStoreFloat4x4(&constants.worldViewProjection, worldViewProjection);

      m_transformBuffer.Update(graphicsDevice, &constants, sizeof(constants));

      object.mesh->Bind(graphicsDevice);
      object.mesh->Draw(graphicsDevice);
    }
  }
}