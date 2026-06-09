#include "Engine/Renderer/GpuBuffer.h"

#include "Engine/Core/Assert.h"
#include "Engine/Core/Log.h"
#include "Engine/Renderer/DxHelpers.h"
#include "Engine/Renderer/GraphicsDevice.h"

#include <cstring>

namespace Engine {
  bool GpuBuffer::CreateVertexBuffer(GraphicsDevice& graphicsDevice, const void* data, uint32_t stride,
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

  bool GpuBuffer::CreateIndexBuffer(GraphicsDevice& graphicsDevice, const uint32_t* indices, uint32_t count) {
    m_stride = sizeof(uint32_t);
    m_count = count;
    m_byteSize = m_stride * count;

    D3D11_BUFFER_DESC bufferDesc = {};
    bufferDesc.ByteWidth = m_byteSize;
    bufferDesc.Usage = D3D11_USAGE_DEFAULT;
    bufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
    bufferDesc.CPUAccessFlags = 0;
    bufferDesc.MiscFlags = 0;
    bufferDesc.StructureByteStride = m_stride;

    D3D11_SUBRESOURCE_DATA initialData = {};
    initialData.pSysMem = indices;

    HRESULT hr = graphicsDevice.GetDevice()->CreateBuffer(&bufferDesc, &initialData, m_buffer.GetAddressOf());

    if (!DX_CHECK(hr)) {
      return false;
    }

    LogInfo("Index buffer created.");
    return true;
  }

  bool GpuBuffer::CreateConstantBuffer(GraphicsDevice& graphicsDevice, uint32_t byteSize) {
    ENGINE_ASSERT(byteSize % 16 == 0, "Constant buffer size must be 16-byte aligned.");

    m_stride = 0;
    m_count = 0;
    m_byteSize = byteSize;

    D3D11_BUFFER_DESC bufferDesc = {};
    bufferDesc.ByteWidth = byteSize;
    bufferDesc.Usage = D3D11_USAGE_DYNAMIC;
    bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    bufferDesc.MiscFlags = 0;
    bufferDesc.StructureByteStride = 0;

    HRESULT hr = graphicsDevice.GetDevice()->CreateBuffer(&bufferDesc, nullptr, m_buffer.GetAddressOf());

    if (!DX_CHECK(hr)) {
      return false;
    }

    LogInfo("Constant buffer created.");
    return true;
  }

  void GpuBuffer::Shutdown() {
    m_buffer.Reset();
    m_stride = 0;
    m_count = 0;
  }

  void GpuBuffer::BindVertexBuffer(GraphicsDevice& graphicsDevice) {
    UINT stride = m_stride;
    UINT offset = 0;

    ID3D11Buffer* buffers[] = {m_buffer.Get()};

    graphicsDevice.GetContext()->IASetVertexBuffers(0, 1, buffers, &stride, &offset);
  }

  void GpuBuffer::BindIndexBuffer(GraphicsDevice& graphicsDevice) {
    graphicsDevice.GetContext()->IASetIndexBuffer(m_buffer.Get(), DXGI_FORMAT_R32_UINT, 0);
  }

  void GpuBuffer::BindConstantBufferVS(GraphicsDevice& graphicsDevice, uint32_t slot) {
    ID3D11Buffer* buffers[] = {m_buffer.Get()};

    graphicsDevice.GetContext()->VSSetConstantBuffers(slot, 1, buffers);
  }

  void GpuBuffer::BindConstantBufferPS(GraphicsDevice& graphicsDevice, uint32_t slot) {
    ID3D11Buffer* buffers[] = {m_buffer.Get()};

    graphicsDevice.GetContext()->PSSetConstantBuffers(slot, 1, buffers);
  }

  void GpuBuffer::Update(GraphicsDevice& graphicsDevice, const void* data, uint32_t byteSize) {
    ENGINE_ASSERT(byteSize <= m_byteSize, "GpuBuffer::Update byteSize is too large.");

    D3D11_MAPPED_SUBRESOURCE mappedResource = {};

    HRESULT hr = graphicsDevice.GetContext()->Map(m_buffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);

    if (!DX_CHECK(hr)) {
      return;
    }

    std::memcpy(mappedResource.pData, data, byteSize);

    graphicsDevice.GetContext()->Unmap(m_buffer.Get(), 0);
  }
}