#pragma once

#include "Engine/Renderer/Material.h"
#include "Engine/Renderer/Mesh.h"
#include "Engine/Renderer/RenderResourceHandles.h"
#include "Engine/Renderer/Texture2D.h"

#include <memory>
#include <vector>

namespace Engine {
  class GraphicsDevice;

  class RenderResourceManager final {
  public:
    bool Initialize(GraphicsDevice& graphicsDevice);
    void Shutdown();

    Mesh* ResolveMesh(MeshHandle handle);
    Material* ResolveMaterial(MaterialHandle handle);
    Texture2D* ResolveTexture(TextureHandle handle);

    MeshHandle GetCubeMeshHandle() const { return m_cubeMeshHandle; }
    MaterialHandle GetColorMaterialHandle() const { return m_colorMaterialHandle; }
    MaterialHandle GetRedMaterialHandle() const { return m_redMaterialHandle; }
    MaterialHandle GetGreenMaterialHandle() const { return m_greenMaterialHandle; }
    MaterialHandle GetBlueMaterialHandle() const { return m_blueMaterialHandle; }
    TextureHandle AddTexture(std::unique_ptr<Texture2D> texture);
    MaterialHandle AddMaterial(std::unique_ptr<Material> material);

  private:
    MeshHandle AddMesh(std::unique_ptr<Mesh> mesh);

  private:
    std::vector<std::unique_ptr<Mesh>> m_meshes;
    std::vector<std::unique_ptr<Material>> m_materials;
    std::vector<std::unique_ptr<Texture2D>> m_textures;
    MaterialHandle m_redMaterialHandle;
    MaterialHandle m_greenMaterialHandle;
    MaterialHandle m_blueMaterialHandle;

    MeshHandle m_cubeMeshHandle;
    MaterialHandle m_colorMaterialHandle;
  };
}