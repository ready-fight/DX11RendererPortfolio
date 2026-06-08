#include "Engine/Renderer/RenderStates.h"

#include "Engine/Core/Log.h"
#include "Engine/Renderer/DxHelpers.h"
#include "Engine/Renderer/GraphicsDevice.h"

namespace Engine {
  bool RenderStates::Initialize(GraphicsDevice& graphicsDevice) {
    D3D11_RASTERIZER_DESC rasterizerDesc = {};
    rasterizerDesc.FillMode = D3D11_FILL_SOLID;
    rasterizerDesc.CullMode = D3D11_CULL_NONE;
    rasterizerDesc.FrontCounterClockwise = FALSE;
    rasterizerDesc.DepthBias = 0;
    rasterizerDesc.DepthBiasClamp = 0.0f;
    rasterizerDesc.SlopeScaledDepthBias = 0.0f;
    rasterizerDesc.DepthClipEnable = TRUE;
    rasterizerDesc.ScissorEnable = FALSE;
    rasterizerDesc.MultisampleEnable = FALSE;
    rasterizerDesc.AntialiasedLineEnable = FALSE;

    HRESULT hr =
        graphicsDevice.GetDevice()->CreateRasterizerState(&rasterizerDesc, m_solidRasterizerState.GetAddressOf());

    if (!DX_CHECK(hr)) {
      return false;
    }

    rasterizerDesc.FillMode = D3D11_FILL_WIREFRAME;

    hr = graphicsDevice.GetDevice()->CreateRasterizerState(&rasterizerDesc, m_wireframeRasterizerState.GetAddressOf());

    if (!DX_CHECK(hr)) {
      return false;
    }

    D3D11_DEPTH_STENCIL_DESC depthDesc = {};
    depthDesc.DepthEnable = TRUE;
    depthDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
    depthDesc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
    depthDesc.StencilEnable = FALSE;

    hr = graphicsDevice.GetDevice()->CreateDepthStencilState(&depthDesc, m_depthEnabledState.GetAddressOf());

    if (!DX_CHECK(hr)) {
      return false;
    }

    LogInfo("Render states initialized.");
    return true;
  }

  void RenderStates::Shutdown() {
    m_depthEnabledState.Reset();
    m_wireframeRasterizerState.Reset();
    m_solidRasterizerState.Reset();
  }

  void RenderStates::Apply(GraphicsDevice& graphicsDevice, bool wireframeEnabled) {
    ID3D11DeviceContext* context = graphicsDevice.GetContext();

    context->RSSetState(wireframeEnabled ? m_wireframeRasterizerState.Get() : m_solidRasterizerState.Get());

    context->OMSetDepthStencilState(m_depthEnabledState.Get(), 0);
  }
}