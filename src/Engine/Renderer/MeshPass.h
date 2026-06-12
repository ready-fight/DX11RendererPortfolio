#pragma once

#include "Engine/Renderer/Camera.h"
#include "Engine/Renderer/GpuBuffer.h"
#include "Engine/Renderer/Material.h"
#include "Engine/Renderer/Mesh.h"
#include "Engine/Renderer/RenderResourceHandles.h"
#include "Engine/Renderer/Texture2D.h"

namespace Engine {
  class GraphicsDevice;
  class Scene;

  class MeshPass final {
  public:
    bool Initialize(GraphicsDevice& graphicsDevice);
    void Shutdown();

    void Render(GraphicsDevice& graphicsDevice, const Camera& camera, Scene& scene, float totalSeconds);

    MeshHandle GetCubeMeshHandle() const { return MeshHandle{0}; }
    MaterialHandle GetColorMaterialHandle() const { return MaterialHandle{0}; }

  private:
    Mesh* ResolveMesh(MeshHandle handle);
    Material* ResolveMaterial(MaterialHandle handle);

  private:
    Material m_colorMaterial;
    Mesh m_cubeMesh;
    Texture2D m_checkerTexture;

    GpuBuffer m_transformBuffer;
    GpuBuffer m_materialBuffer;
    GpuBuffer m_lightBuffer;
  };
}