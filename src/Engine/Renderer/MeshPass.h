#pragma once

#include "Engine/Renderer/Camera.h"
#include "Engine/Renderer/GpuBuffer.h"
#include "Engine/Renderer/Material.h"
#include "Engine/Renderer/Mesh.h"
#include "Engine/Renderer/RenderResourceHandles.h"
#include "Engine/Renderer/Texture2D.h"

#include <memory>
#include <vector>

namespace Engine {
  class GraphicsDevice;
  class Scene;

  class MeshPass final {
  public:
    bool Initialize(GraphicsDevice& graphicsDevice);
    void Shutdown();

    void Render(GraphicsDevice& graphicsDevice, const Camera& camera, Scene& scene, float totalSeconds);

    MeshHandle GetCubeMeshHandle() const { return m_cubeMeshHandle; }
    MaterialHandle GetColorMaterialHandle() const { return m_colorMaterialHandle; }

  private:
    MeshHandle AddMesh(std::unique_ptr<Mesh> mesh);
    MaterialHandle AddMaterial(std::unique_ptr<Material> material);

    Mesh* ResolveMesh(MeshHandle handle);
    Material* ResolveMaterial(MaterialHandle handle);

  private:
    std::vector<std::unique_ptr<Mesh>> m_meshes;
    std::vector<std::unique_ptr<Material>> m_materials;

    MeshHandle m_cubeMeshHandle;
    MaterialHandle m_colorMaterialHandle;

    Texture2D m_checkerTexture;

    GpuBuffer m_transformBuffer;
    GpuBuffer m_materialBuffer;
    GpuBuffer m_lightBuffer;
  };
}