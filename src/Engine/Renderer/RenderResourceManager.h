#pragma once

#include "Engine/Renderer/Material.h"
#include "Engine/Renderer/MaterialTypes.h"
#include "Engine/Renderer/Mesh.h"
#include "Engine/Renderer/RenderResourceHandles.h"
#include "Engine/Renderer/Texture2D.h"

#include <memory>
#include <vector>

namespace Engine {
  class GraphicsDevice;
  struct MeshData;

  class RenderResourceManager final {
  public:
    bool Initialize(GraphicsDevice& graphicsDevice);
    void Shutdown();

    TextureHandle CreateTexture(GraphicsDevice& graphicsDevice, const wchar_t* filePath, const char* debugName);
    MaterialHandle CreateMaterial(GraphicsDevice& graphicsDevice, const MaterialDesc& desc);

    Mesh* ResolveMesh(MeshHandle handle);
    Material* ResolveMaterial(MaterialHandle handle);
    Texture2D* ResolveTexture(TextureHandle handle);

    MeshHandle GetCubeMeshHandle() const { return m_cubeMeshHandle; }
    MaterialHandle GetColorMaterialHandle() const { return m_colorMaterialHandle; }
    MaterialHandle GetRedMaterialHandle() const { return m_redMaterialHandle; }
    MaterialHandle GetGreenMaterialHandle() const { return m_greenMaterialHandle; }
    MaterialHandle GetBlueMaterialHandle() const { return m_blueMaterialHandle; }

    const char* GetMaterialDebugName(MaterialHandle handle) const;
    MaterialShaderType GetMaterialShaderType(MaterialHandle handle) const;

    uint32_t GetMaterialCount() const { return static_cast<uint32_t>(m_materials.size()); }

    MaterialHandle GetMaterialHandleAt(uint32_t index) const;

    const char* GetTextureDebugName(TextureHandle handle) const;
    uint32_t GetTextureWidth(TextureHandle handle) const;
    uint32_t GetTextureHeight(TextureHandle handle) const;
    TextureHandle GetMaterialBaseTexture(MaterialHandle handle) const;
    MeshHandle GetModelMeshHandle() const { return m_modelMeshHandle; }

    MeshHandle CreateCubeMesh(GraphicsDevice& graphicsDevice, const char* debugName);
    MeshHandle CreateMeshFromData(GraphicsDevice& graphicsDevice, const MeshData& meshData, const char* debugName);
    MeshHandle CreateModelMesh(GraphicsDevice& graphicsDevice, const char* filePath, const char* debugName);

    const char* GetMeshDebugName(MeshHandle handle) const;
    uint32_t GetMeshVertexCount(MeshHandle handle) const;
    uint32_t GetMeshIndexCount(MeshHandle handle) const;

  private:
    MeshHandle AddMesh(std::unique_ptr<Mesh> mesh);
    TextureHandle AddTexture(std::unique_ptr<Texture2D> texture);
    MaterialHandle AddMaterial(std::unique_ptr<Material> material);

  private:
    std::vector<std::unique_ptr<Mesh>> m_meshes;
    std::vector<std::unique_ptr<Material>> m_materials;
    std::vector<std::unique_ptr<Texture2D>> m_textures;

    MaterialHandle m_redMaterialHandle;
    MaterialHandle m_greenMaterialHandle;
    MaterialHandle m_blueMaterialHandle;

    MeshHandle m_cubeMeshHandle;
    MaterialHandle m_colorMaterialHandle;
    MeshHandle m_modelMeshHandle;
  };
}