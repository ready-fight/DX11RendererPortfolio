#pragma once

#include "Engine/Renderer/GpuBuffer.h"
#include "Engine/Renderer/MeshData.h"

#include <cstdint>

namespace Engine {
  class GraphicsDevice;
  struct VertexPositionColor;

  class Mesh final {
  public:
    bool Initialize(GraphicsDevice& graphicsDevice, const VertexPositionColor* vertices, uint32_t vertexCount,
                    const uint32_t* indices, uint32_t indexCount);

    bool Initialize(GraphicsDevice& graphicsDevice, const MeshData& meshData);

    void Shutdown();

    void Bind(GraphicsDevice& graphicsDevice);
    void Draw(GraphicsDevice& graphicsDevice);

    uint32_t GetIndexCount() const { return m_indexBuffer.GetCount(); }

  private:
    GpuBuffer m_vertexBuffer;
    GpuBuffer m_indexBuffer;
  };
}