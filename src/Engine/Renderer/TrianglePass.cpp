#include "Engine/Renderer/TrianglePass.h"

#include "Engine/Renderer/GraphicsDevice.h"
#include "Engine/Renderer/VertexTypes.h"

#include <cstddef>
#include <iterator>

namespace Engine {
  bool TrianglePass::Initialize(GraphicsDevice &graphicsDevice) {
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

    return m_vertexBuffer.CreateVertexBuffer(
        graphicsDevice, vertices, sizeof(VertexPositionColor), static_cast<uint32_t>(std::size(vertices)));
  }

  void TrianglePass::Shutdown()
    {
        m_vertexBuffer.Shutdown();
        m_shader.Shutdown();
    }

    void TrianglePass::Render(GraphicsDevice& graphicsDevice)
    {
        ID3D11DeviceContext* context = graphicsDevice.GetContext();

        m_shader.Bind(graphicsDevice);
        m_vertexBuffer.BindVertexBuffer(graphicsDevice);

        context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
        context->Draw(m_vertexBuffer.GetCount(), 0);
    }

}