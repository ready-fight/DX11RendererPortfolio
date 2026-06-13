#pragma once

#include "Engine/Renderer/Material.h"
#include "Engine/Renderer/Mesh.h"
#include "Engine/Renderer/RenderResourceHandles.h"

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

    MeshHandle GetCubeMeshHandle() const { return m_cubeMeshHandle; }
    MaterialHandle GetColorMaterialHandle() const { return m_colorMaterialHandle; }

  private:
    MeshHandle AddMesh(std::unique_ptr<Mesh> mesh);
    MaterialHandle AddMaterial(std::unique_ptr<Material> material);

  private:
    std::vector<std::unique_ptr<Mesh>> m_meshes;
    std::vector<std::unique_ptr<Material>> m_materials;

    MeshHandle m_cubeMeshHandle;
    MaterialHandle m_colorMaterialHandle;
  };
}