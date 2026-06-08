#include "Engine/Renderer/MeshPass.h"

#include "Engine/Renderer/GraphicsDevice.h"
#include "Engine/Renderer/MeshFactory.h"
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

    if (!MeshFactory::CreateCube(graphicsDevice, m_cubeMesh)) {
      return false;
    }

    if (!m_transformBuffer.CreateConstantBuffer(graphicsDevice, sizeof(TransformConstants))) {
      return false;
    }

    return true;
  }

  void MeshPass::Shutdown() {
    m_transformBuffer.Shutdown();
    m_cubeMesh.Shutdown();
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
    m_cubeMesh.Bind(graphicsDevice);
    m_transformBuffer.BindConstantBufferVS(graphicsDevice, 0);

    context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    m_cubeMesh.Draw(graphicsDevice);
  }
}