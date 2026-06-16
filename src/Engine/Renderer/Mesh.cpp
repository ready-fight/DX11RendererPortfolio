#include "Engine/Renderer/Mesh.h"

#include "Engine/Renderer/GraphicsDevice.h"
#include "Engine/Renderer/VertexTypes.h"

namespace Engine {
  bool Mesh::Initialize(GraphicsDevice& graphicsDevice, const VertexPositionColor* vertices, uint32_t vertexCount,
                        const uint32_t* indices, uint32_t indexCount) {

    m_vertexCount = vertexCount;
    m_indexCount = indexCount;

    if (!m_vertexBuffer.CreateVertexBuffer(graphicsDevice, vertices, sizeof(VertexPositionColor), vertexCount)) {
      return false;
    }

    if (!m_indexBuffer.CreateIndexBuffer(graphicsDevice, indices, indexCount)) {
      return false;
    }

    return true;
  }

  bool Mesh::Initialize(GraphicsDevice& graphicsDevice, const MeshData& meshData) {

    if (!meshData.IsValid()) {
      return false;
    }

    return Initialize(graphicsDevice,
                      meshData.vertices.data(),
                      static_cast<uint32_t>(meshData.vertices.size()),
                      meshData.indices.data(),
                      static_cast<uint32_t>(meshData.indices.size()));
  }

  void Mesh::Shutdown() {
    m_indexBuffer.Shutdown();
    m_vertexBuffer.Shutdown();

    m_debugName = "Unnamed Mesh";
    m_vertexCount = 0;
    m_indexCount = 0;
  }

  void Mesh::Bind(GraphicsDevice& graphicsDevice) {
    m_vertexBuffer.BindVertexBuffer(graphicsDevice);
    m_indexBuffer.BindIndexBuffer(graphicsDevice);
  }

  void Mesh::Draw(GraphicsDevice& graphicsDevice) { graphicsDevice.DrawIndexed(m_indexBuffer.GetCount(), 0, 0); }
}