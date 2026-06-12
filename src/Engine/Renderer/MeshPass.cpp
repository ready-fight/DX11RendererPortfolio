#include "Engine/Renderer/MeshPass.h"

#include "Engine/Renderer/GraphicsDevice.h"
#include "Engine/Renderer/MeshFactory.h"
#include "Engine/Renderer/VertexTypes.h"
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
                                                 0},
                                                {"NORMAL",
                                                 0,
                                                 DXGI_FORMAT_R32G32B32_FLOAT,
                                                 0,
                                                 static_cast<UINT>(offsetof(VertexPositionColor, normal)),
                                                 D3D11_INPUT_PER_VERTEX_DATA,
                                                 0},
                                                {"TEXCOORD",
                                                 0,
                                                 DXGI_FORMAT_R32G32_FLOAT,
                                                 0,
                                                 static_cast<UINT>(offsetof(VertexPositionColor, texcoord)),
                                                 D3D11_INPUT_PER_VERTEX_DATA,
                                                 0}};

    auto colorMaterial = std::make_unique<Material>();

    if (!colorMaterial->Initialize(graphicsDevice,
                                   L"assets/shaders/Color.hlsl",
                                   inputElements,
                                   static_cast<unsigned int>(std::size(inputElements)))) {
      return false;
    }

    m_colorMaterialHandle = AddMaterial(std::move(colorMaterial));

    auto cubeMesh = std::make_unique<Mesh>();

    if (!MeshFactory::CreateCube(graphicsDevice, *cubeMesh)) {
      return false;
    }

    m_cubeMeshHandle = AddMesh(std::move(cubeMesh));

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

    for (std::unique_ptr<Mesh>& mesh : m_meshes) {
      if (mesh) {
        mesh->Shutdown();
      }
    }

    for (std::unique_ptr<Material>& material : m_materials) {
      if (material) {
        material->Shutdown();
      }
    }

    m_meshes.clear();
    m_materials.clear();

    m_cubeMeshHandle = {};
    m_colorMaterialHandle = {};
  }

  void MeshPass::Render(GraphicsDevice& graphicsDevice, const Camera& camera, Scene& scene, float totalSeconds) {
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

      Mesh* mesh = ResolveMesh(object.mesh);
      Material* material = ResolveMaterial(object.material);

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

  MeshHandle MeshPass::AddMesh(std::unique_ptr<Mesh> mesh) {
    const uint32_t index = static_cast<uint32_t>(m_meshes.size());
    m_meshes.push_back(std::move(mesh));
    return MeshHandle{index};
  }

  MaterialHandle MeshPass::AddMaterial(std::unique_ptr<Material> material) {
    const uint32_t index = static_cast<uint32_t>(m_materials.size());
    m_materials.push_back(std::move(material));
    return MaterialHandle{index};
  }

  Mesh* MeshPass::ResolveMesh(MeshHandle handle) {
    if (!handle.IsValid() || handle.value >= m_meshes.size()) {
      return nullptr;
    }

    return m_meshes[handle.value].get();
  }

  Material* MeshPass::ResolveMaterial(MaterialHandle handle) {
    if (!handle.IsValid() || handle.value >= m_materials.size()) {
      return nullptr;
    }

    return m_materials[handle.value].get();
  }
}