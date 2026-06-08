#include "Engine/Renderer/Mesh.h"

#include "Engine/Renderer/GraphicsDevice.h"
#include "Engine/Renderer/VertexTypes.h"

namespace Engine {
  bool Mesh::Initialize(GraphicsDevice& graphicsDevice, const VertexPositionColor* vertices, uint32_t vertexCount,
                        const uint32_t* indices, uint32_t indexCount) {
    if (!m_vertexBuffer.CreateVertexBuffer(graphicsDevice, vertices, sizeof(VertexPositionColor), vertexCount)) {
      return false;
    }

    if (!m_indexBuffer.CreateIndexBuffer(graphicsDevice, indices, indexCount)) {
      return false;
    }

    return true;
  }

  void Mesh::Shutdown() {
    m_indexBuffer.Shutdown();
    m_vertexBuffer.Shutdown();
  }

  void Mesh::Bind(GraphicsDevice& graphicsDevice) {
    m_vertexBuffer.BindVertexBuffer(graphicsDevice);
    m_indexBuffer.BindIndexBuffer(graphicsDevice);
  }

  void Mesh::Draw(GraphicsDevice& graphicsDevice) {
    graphicsDevice.GetContext()->DrawIndexed(m_indexBuffer.GetCount(), 0, 0);
  }
}