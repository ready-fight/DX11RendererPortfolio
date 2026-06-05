#include "Engine/Renderer/TrianglePass.h"

#include "Engine/Renderer/GraphicsDevice.h"
#include "Engine/Renderer/VertexTypes.h"

#include <DirectXMath.h>
#include <cstddef>
#include <iterator>

#include <iostream>
#include <string>

namespace Engine {

  namespace {
    struct TransformConstants {
      DirectX::XMFLOAT4X4 worldViewProjection;
    };

    static_assert(sizeof(TransformConstants) % 16 == 0);
  }

  bool TrianglePass::Initialize(GraphicsDevice& graphicsDevice) {
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
                             L"assets/shaders/Triangle.hlsl",
                             inputElements,
                             static_cast<unsigned int>(std::size(inputElements)))) {
      return false;
    }

    const VertexPositionColor vertices[] = {{{0.0f, 0.5f, 0.0f}, {1.0f, 0.0f, 0.0f}},
                                            {{0.5f, -0.5f, 0.0f}, {0.0f, 1.0f, 0.0f}},
                                            {{-0.5f, -0.5f, 0.0f}, {0.0f, 0.4f, 1.0f}}};

    if (!m_vertexBuffer.CreateVertexBuffer(
            graphicsDevice, vertices, sizeof(VertexPositionColor), static_cast<uint32_t>(std::size(vertices)))) {
      return false;
    }

    if (!m_transformBuffer.CreateConstantBuffer(graphicsDevice, sizeof(TransformConstants))) {
      return false;
    }

    return true;
  }

  void TrianglePass::Shutdown() {
    m_transformBuffer.Shutdown();
    m_vertexBuffer.Shutdown();
    m_shader.Shutdown();
  }

  void TrianglePass::Render(GraphicsDevice& graphicsDevice, float totalSeconds) {

    const DirectX::XMMATRIX rotation = DirectX::XMMatrixRotationZ(totalSeconds);

    TransformConstants constants = {};
    DirectX::XMStoreFloat4x4(&constants.worldViewProjection, rotation);

    m_transformBuffer.Update(graphicsDevice, &constants, sizeof(constants));

    ID3D11DeviceContext* context = graphicsDevice.GetContext();

    m_shader.Bind(graphicsDevice);
    m_vertexBuffer.BindVertexBuffer(graphicsDevice);
    m_transformBuffer.BindConstantBufferVS(graphicsDevice, 0);

    context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    context->Draw(m_vertexBuffer.GetCount(), 0);
  }

}