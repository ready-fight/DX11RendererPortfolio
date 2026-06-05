#include "Engine/Renderer/GpuBuffer.h"

#include "Engine/Core/Log.h"
#include "Engine/Renderer/DxHelpers.h"
#include "Engine/Renderer/GraphicsDevice.h"

namespace Engine {
  bool GpuBuffer::CreateVertexBuffer(GraphicsDevice &graphicsDevice, const void *data, uint32_t stride,
                                     uint32_t count) {
    m_stride = stride;
    m_count = count;

    D3D11_BUFFER_DESC bufferDesc = {};
    bufferDesc.ByteWidth = stride * count;
    bufferDesc.Usage = D3D11_USAGE_DEFAULT;
    bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    bufferDesc.CPUAccessFlags = 0;
    bufferDesc.MiscFlags = 0;
    bufferDesc.StructureByteStride = stride;

    D3D11_SUBRESOURCE_DATA initialData = {};
    initialData.pSysMem = data;

    HRESULT hr = graphicsDevice.GetDevice()->CreateBuffer(&bufferDesc, &initialData, m_buffer.GetAddressOf());

    if (!DX_CHECK(hr)) {
      return false;
    }

    LogInfo("Vertex buffer created.");
    return true;
  }

  void GpuBuffer::Shutdown() {
    m_buffer.Reset();
    m_stride = 0;
    m_count = 0;
  }

  void GpuBuffer::BindVertexBuffer(GraphicsDevice &graphicsDevice) {
    UINT stride = m_stride;
    UINT offset = 0;

    ID3D11Buffer *buffers[] = {m_buffer.Get()};

    graphicsDevice.GetContext()->IASetVertexBuffers(0, 1, buffers, &stride, &offset);
  }
}