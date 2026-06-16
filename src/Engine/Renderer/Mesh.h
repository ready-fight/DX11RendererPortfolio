#pragma once

#include "Engine/Renderer/GpuBuffer.h"
#include "Engine/Renderer/MeshData.h"

#include <cstdint>

#include <string>

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

    void SetDebugName(const std::string& debugName) { m_debugName = debugName; }

    const std::string& GetDebugName() const { return m_debugName; }

    uint32_t GetVertexCount() const { return m_vertexCount; }

    uint32_t GetMeshIndexCount() const { return m_indexCount; }

  private:
    GpuBuffer m_vertexBuffer;
    GpuBuffer m_indexBuffer;

    std::string m_debugName = "Unnamed Mesh";
    uint32_t m_vertexCount = 0;
    uint32_t m_indexCount = 0;
  };
}