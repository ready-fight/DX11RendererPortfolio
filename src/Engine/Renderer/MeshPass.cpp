#include "Engine/Renderer/MeshPass.h"

#include "Engine/Renderer/GraphicsDevice.h"
#include "Engine/Renderer/VertexTypes.h"

#include <DirectXMath.h>
#include <cstddef>
#include <iterator>

namespace Engine {
  namespace {
    struct TransformConstants {
      DirectX::XMFLOAT4X4 worldViewProjection;
    };

    static_assert(sizeof(TransformConstants) % 16 == 0);
  }

  bool MeshPass::Initialize(GraphicsDevice& graphicsDevice) {
    D3D11_INPUT_ELEMENT_DESC inputElements[] = {{"POSITION",
                                                 0,
                                                 DXGI_FORMAT_R32G32B32_FLOAT,
                                                 0,
                                                 static_cast<UINT>(offsetof(VertexPositionColor, position)),
                                                 D3D11_INPUT_PER_VERTEX_DATA,
                                                 0},
                                                {"COLOR",
                                                 0,
                                                 DXGI_FORMAT_R32G32B32_FLOAT,
                                                 0,
                                                 static_cast<UINT>(offsetof(VertexPositionColor, color)),
                                                 D3D11_INPUT_PER_VERTEX_DATA,
                                                 0}};

    if (!m_shader.Initialize(graphicsDevice,
                             L"assets/shaders/Color.hlsl",
                             inputElements,
                             static_cast<unsigned int>(std::size(inputElements)))) {
      return false;
    }

    const VertexPositionColor vertices[] = {{{-0.5f, -0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}},
                                            {{-0.5f, 0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}},
                                            {{0.5f, 0.5f, -0.5f}, {0.0f, 0.0f, 1.0f}},
                                            {{0.5f, -0.5f, -0.5f}, {1.0f, 1.0f, 0.0f}},

                                            {{-0.5f, -0.5f, 0.5f}, {1.0f, 0.0f, 1.0f}},
                                            {{-0.5f, 0.5f, 0.5f}, {0.0f, 1.0f, 1.0f}},
                                            {{0.5f, 0.5f, 0.5f}, {1.0f, 1.0f, 1.0f}},
                                            {{0.5f, -0.5f, 0.5f}, {0.3f, 0.3f, 0.3f}}};

    const uint32_t indices[] = {0, 1, 2, 0, 2, 3, 4, 6, 5, 4, 7, 6,

                                4, 5, 1, 4, 1, 0, 3, 2, 6, 3, 6, 7,

                                1, 5, 6, 1, 6, 2, 4, 0, 3, 4, 3, 7};

    if (!m_vertexBuffer.CreateVertexBuffer(
            graphicsDevice, vertices, sizeof(VertexPositionColor), static_cast<uint32_t>(std::size(vertices)))) {
      return false;
    }

    if (!m_indexBuffer.CreateIndexBuffer(graphicsDevice, indices, static_cast<uint32_t>(std::size(indices)))) {
      return false;
    }

    if (!m_transformBuffer.CreateConstantBuffer(graphicsDevice, sizeof(TransformConstants))) {
      return false;
    }

    return true;
  }

  void MeshPass::Shutdown() {
    m_transformBuffer.Shutdown();
    m_indexBuffer.Shutdown();
    m_vertexBuffer.Shutdown();
    m_shader.Shutdown();
  }

  void MeshPass::Render(GraphicsDevice& graphicsDevice, const Camera& camera, float totalSeconds) {
    using namespace DirectX;

    const XMMATRIX world = XMMatrixRotationX(totalSeconds * 0.5f) * XMMatrixRotationY(totalSeconds);

    const XMMATRIX viewProjection = camera.GetViewProjectionMatrix();
    const XMMATRIX worldViewProjection = world * viewProjection;

    TransformConstants constants = {};
    XMStoreFloat4x4(&constants.worldViewProjection, worldViewProjection);

    m_transformBuffer.Update(graphicsDevice, &constants, sizeof(constants));

    ID3D11DeviceContext* context = graphicsDevice.GetContext();

    m_shader.Bind(graphicsDevice);
    m_vertexBuffer.BindVertexBuffer(graphicsDevice);
    m_indexBuffer.BindIndexBuffer(graphicsDevice);
    m_transformBuffer.BindConstantBufferVS(graphicsDevice, 0);

    context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    context->DrawIndexed(m_indexBuffer.GetCount(), 0, 0);
  }
}