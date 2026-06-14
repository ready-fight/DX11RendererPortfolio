// src/Engine/Renderer/RenderResourceManager.cpp
#include "Engine/Renderer/RenderResourceManager.h"

#include "Engine/Renderer/MeshFactory.h"
#include "Engine/Renderer/VertexTypes.h"

#include "Engine/Core/Log.h"
#include "Engine/Renderer/Texture2D.h"

#include <cstddef>

namespace Engine {
  bool RenderResourceManager::Initialize(GraphicsDevice& graphicsDevice) {

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

    auto baseTexture = std::make_unique<Texture2D>();

    if (!baseTexture->LoadFromFile(graphicsDevice, L"assets/textures/test.png")) {
      LogWarning("Failed to load texture file. Falling back to checkerboard texture.");

      if (!baseTexture->CreateCheckerboard(graphicsDevice, 64, 64)) {
        return false;
      }
    }

    TextureHandle baseTextureHandle = AddTexture(std::move(baseTexture));

    auto colorMaterial = std::make_unique<Material>();

    if (!colorMaterial->Initialize(graphicsDevice,
                                   L"assets/shaders/Color.hlsl",
                                   inputElements,
                                   static_cast<unsigned int>(std::size(inputElements)))) {
      return false;
    }

    colorMaterial->SetBaseTexture(baseTextureHandle);

    m_colorMaterialHandle = AddMaterial(std::move(colorMaterial));

    auto cubeMesh = std::make_unique<Mesh>();

    if (!MeshFactory::CreateCube(graphicsDevice, *cubeMesh)) {
      return false;
    }

    m_cubeMeshHandle = AddMesh(std::move(cubeMesh));

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
}