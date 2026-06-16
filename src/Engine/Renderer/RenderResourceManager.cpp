#include "Engine/Renderer/RenderResourceManager.h"

#include "Engine/Renderer/MeshData.h"
#include "Engine/Renderer/MeshFactory.h"
#include "Engine/Renderer/ObjLoader.h"
#include "Engine/Renderer/VertexTypes.h"

#include "Engine/Core/Log.h"
#include "Engine/Renderer/Texture2D.h"

#include <cstddef>

namespace Engine {

  namespace {
    void BuildVertexInputLayout(D3D11_INPUT_ELEMENT_DESC* inputElements) {
      inputElements[0] = {"POSITION",
                          0,
                          DXGI_FORMAT_R32G32B32_FLOAT,
                          0,
                          static_cast<UINT>(offsetof(VertexPositionColor, position)),
                          D3D11_INPUT_PER_VERTEX_DATA,
                          0};

      inputElements[1] = {"COLOR",
                          0,
                          DXGI_FORMAT_R32G32B32_FLOAT,
                          0,
                          static_cast<UINT>(offsetof(VertexPositionColor, color)),
                          D3D11_INPUT_PER_VERTEX_DATA,
                          0};

      inputElements[2] = {"NORMAL",
                          0,
                          DXGI_FORMAT_R32G32B32_FLOAT,
                          0,
                          static_cast<UINT>(offsetof(VertexPositionColor, normal)),
                          D3D11_INPUT_PER_VERTEX_DATA,
                          0};

      inputElements[3] = {"TEXCOORD",
                          0,
                          DXGI_FORMAT_R32G32_FLOAT,
                          0,
                          static_cast<UINT>(offsetof(VertexPositionColor, texcoord)),
                          D3D11_INPUT_PER_VERTEX_DATA,
                          0};
    }
  }

  TextureHandle RenderResourceManager::CreateTexture(GraphicsDevice& graphicsDevice, const wchar_t* filePath,
                                                     const char* debugName) {
    auto texture = std::make_unique<Texture2D>();

    texture->SetDebugName(debugName);

    if (!texture->LoadFromFile(graphicsDevice, filePath)) {
      LogWarning("Failed to load texture file. Falling back to checkerboard texture.");

      if (!texture->CreateCheckerboard(graphicsDevice, 64, 64)) {
        return {};
      }
    }

    return AddTexture(std::move(texture));
  }

  MaterialHandle RenderResourceManager::CreateMaterial(GraphicsDevice& graphicsDevice, const MaterialDesc& desc) {
    D3D11_INPUT_ELEMENT_DESC inputElements[4] = {};
    BuildVertexInputLayout(inputElements);

    auto material = std::make_unique<Material>();

    if (!material->Initialize(
            graphicsDevice, desc, inputElements, static_cast<unsigned int>(std::size(inputElements)))) {
      return {};
    }

    return AddMaterial(std::move(material));
  }

  bool RenderResourceManager::Initialize(GraphicsDevice& graphicsDevice) {
    TextureHandle textureA = CreateTexture(graphicsDevice, L"assets/textures/test.jpg", "Test Texture A");

    TextureHandle textureB = CreateTexture(graphicsDevice, L"assets/textures/test_b.png", "Test Texture B");

    TextureHandle textureC = CreateTexture(graphicsDevice, L"assets/textures/test_c.jpg", "Test Texture C");

    MaterialDesc redMaterialDesc = {};
    redMaterialDesc.debugName = "Red Lit Material";
    redMaterialDesc.shaderType = MaterialShaderType::LitTextured;
    redMaterialDesc.baseTexture = textureA;

    MaterialDesc greenMaterialDesc = {};
    greenMaterialDesc.debugName = "Green Unlit Material";
    greenMaterialDesc.shaderType = MaterialShaderType::UnlitTextured;
    greenMaterialDesc.baseTexture = textureB;

    MaterialDesc blueMaterialDesc = {};
    blueMaterialDesc.debugName = "Blue Lit Material";
    blueMaterialDesc.shaderType = MaterialShaderType::LitTextured;
    blueMaterialDesc.baseTexture = textureC;

    m_redMaterialHandle = CreateMaterial(graphicsDevice, redMaterialDesc);
    m_greenMaterialHandle = CreateMaterial(graphicsDevice, greenMaterialDesc);
    m_blueMaterialHandle = CreateMaterial(graphicsDevice, blueMaterialDesc);

    m_colorMaterialHandle = m_redMaterialHandle;

    m_cubeMeshHandle = CreateCubeMesh(graphicsDevice, "Cube Mesh");

    if (!m_cubeMeshHandle.IsValid()) {
      return false;
    }

    m_modelMeshHandle = CreateModelMesh(graphicsDevice, "assets/models/pyramid.obj", "Pyramid Mesh");

    if (!m_modelMeshHandle.IsValid()) {
      LogWarning("Failed to create model mesh. Falling back to cube mesh.");

      m_modelMeshHandle = m_cubeMeshHandle;
    }

    return true;
  }

  void RenderResourceManager::Shutdown() {
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

    for (std::unique_ptr<Texture2D>& texture : m_textures) {
      if (texture) {
        texture->Shutdown();
      }
    }

    m_meshes.clear();
    m_materials.clear();
    m_textures.clear();

    m_cubeMeshHandle = {};
    m_colorMaterialHandle = {};
    m_modelMeshHandle = {};
    m_redMaterialHandle = {};
    m_greenMaterialHandle = {};
    m_blueMaterialHandle = {};
  }

  MeshHandle RenderResourceManager::AddMesh(std::unique_ptr<Mesh> mesh) {
    const uint32_t index = static_cast<uint32_t>(m_meshes.size());
    m_meshes.push_back(std::move(mesh));
    return MeshHandle{index};
  }

  MaterialHandle RenderResourceManager::AddMaterial(std::unique_ptr<Material> material) {
    const uint32_t index = static_cast<uint32_t>(m_materials.size());
    m_materials.push_back(std::move(material));
    return MaterialHandle{index};
  }

  TextureHandle RenderResourceManager::AddTexture(std::unique_ptr<Texture2D> texture) {
    const uint32_t index = static_cast<uint32_t>(m_textures.size());
    m_textures.push_back(std::move(texture));
    return TextureHandle{index};
  }

  Mesh* RenderResourceManager::ResolveMesh(MeshHandle handle) {
    if (!handle.IsValid() || handle.value >= m_meshes.size()) {
      return nullptr;
    }

    return m_meshes[handle.value].get();
  }

  Material* RenderResourceManager::ResolveMaterial(MaterialHandle handle) {
    if (!handle.IsValid() || handle.value >= m_materials.size()) {
      return nullptr;
    }

    return m_materials[handle.value].get();
  }

  Texture2D* RenderResourceManager::ResolveTexture(TextureHandle handle) {
    if (!handle.IsValid() || handle.value >= m_textures.size()) {
      return nullptr;
    }

    return m_textures[handle.value].get();
  }

  const char* RenderResourceManager::GetMaterialDebugName(MaterialHandle handle) const {
    if (!handle.IsValid() || handle.value >= m_materials.size()) {
      return "Invalid Material";
    }

    return m_materials[handle.value]->GetDebugName().c_str();
  }

  MaterialShaderType RenderResourceManager::GetMaterialShaderType(MaterialHandle handle) const {
    if (!handle.IsValid() || handle.value >= m_materials.size()) {
      return MaterialShaderType::LitTextured;
    }

    return m_materials[handle.value]->GetShaderType();
  }

  MaterialHandle RenderResourceManager::GetMaterialHandleAt(uint32_t index) const {
    if (index >= m_materials.size()) {
      return {};
    }

    return MaterialHandle{index};
  }

  const char* RenderResourceManager::GetTextureDebugName(TextureHandle handle) const {
    if (!handle.IsValid() || handle.value >= m_textures.size()) {
      return "Invalid Texture";
    }

    return m_textures[handle.value]->GetDebugName().c_str();
  }

  uint32_t RenderResourceManager::GetTextureWidth(TextureHandle handle) const {
    if (!handle.IsValid() || handle.value >= m_textures.size()) {
      return 0;
    }

    return m_textures[handle.value]->GetWidth();
  }

  uint32_t RenderResourceManager::GetTextureHeight(TextureHandle handle) const {
    if (!handle.IsValid() || handle.value >= m_textures.size()) {
      return 0;
    }

    return m_textures[handle.value]->GetHeight();
  }

  TextureHandle RenderResourceManager::GetMaterialBaseTexture(MaterialHandle handle) const {
    if (!handle.IsValid() || handle.value >= m_materials.size()) {
      return {};
    }

    return m_materials[handle.value]->GetBaseTextureHandle();
  }

  MeshHandle RenderResourceManager::CreateCubeMesh(GraphicsDevice& graphicsDevice, const char* debugName) {
    MeshData meshData = {};

    if (!MeshFactory::CreateCubeMeshData(meshData)) {
      LogWarning("Failed to create cube mesh data.");
      return {};
    }

    return CreateMeshFromData(graphicsDevice, meshData, debugName);
  }

  MeshHandle RenderResourceManager::CreateMeshFromData(GraphicsDevice& graphicsDevice, const MeshData& meshData,
                                                       const char* debugName) {
    auto mesh = std::make_unique<Mesh>();
    mesh->SetDebugName(debugName);

    if (!mesh->Initialize(graphicsDevice, meshData)) {
      LogWarning("Failed to create mesh from mesh data.");
      return {};
    }

    return AddMesh(std::move(mesh));
  }

  MeshHandle RenderResourceManager::CreateModelMesh(GraphicsDevice& graphicsDevice, const char* filePath,
                                                    const char* debugName) {
    MeshData meshData = {};

    if (!ObjLoader::Load(filePath, meshData)) {
      LogWarning("Failed to load model mesh.");
      return {};
    }

    return CreateMeshFromData(graphicsDevice, meshData, debugName);
  }

  const char* RenderResourceManager::GetMeshDebugName(MeshHandle handle) const {
    if (!handle.IsValid() || handle.value >= m_meshes.size()) {
      return "Invalid Mesh";
    }

    return m_meshes[handle.value]->GetDebugName().c_str();
  }

  uint32_t RenderResourceManager::GetMeshVertexCount(MeshHandle handle) const {
    if (!handle.IsValid() || handle.value >= m_meshes.size()) {
      return 0;
    }

    return m_meshes[handle.value]->GetVertexCount();
  }

  uint32_t RenderResourceManager::GetMeshIndexCount(MeshHandle handle) const {
    if (!handle.IsValid() || handle.value >= m_meshes.size()) {
      return 0;
    }

    return m_meshes[handle.value]->GetIndexCount();
  }
}