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

    D3D11_BLEND_DESC opaqueBlendDesc = {};
    opaqueBlendDesc.RenderTarget[0].BlendEnable = FALSE;
    opaqueBlendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

    hr = graphicsDevice.GetDevice()->CreateBlendState(&opaqueBlendDesc, m_opaqueBlendState.GetAddressOf());

    if (!DX_CHECK(hr)) {
      return false;
    }

    D3D11_BLEND_DESC alphaBlendDesc = {};
    alphaBlendDesc.RenderTarget[0].BlendEnable = TRUE;
    alphaBlendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
    alphaBlendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
    alphaBlendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
    alphaBlendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
    alphaBlendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_INV_SRC_ALPHA;
    alphaBlendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
    alphaBlendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

    hr = graphicsDevice.GetDevice()->CreateBlendState(&alphaBlendDesc, m_alphaBlendState.GetAddressOf());

    if (!DX_CHECK(hr)) {
      return false;
    }

    D3D11_BLEND_DESC additiveBlendDesc = {};
    additiveBlendDesc.RenderTarget[0].BlendEnable = TRUE;
    additiveBlendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
    additiveBlendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_ONE;
    additiveBlendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
    additiveBlendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
    additiveBlendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ONE;
    additiveBlendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
    additiveBlendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

    hr = graphicsDevice.GetDevice()->CreateBlendState(&additiveBlendDesc, m_additiveBlendState.GetAddressOf());

    if (!DX_CHECK(hr)) {
      return false;
    }

    LogInfo("Render states initialized.");
    return true;
  }

  void RenderStates::Shutdown() {
    m_additiveBlendState.Reset();
    m_alphaBlendState.Reset();
    m_opaqueBlendState.Reset();

    m_depthEnabledState.Reset();

    m_wireframeRasterizerState.Reset();
    m_solidRasterizerState.Reset();
  }

  void RenderStates::Apply(GraphicsDevice& graphicsDevice, bool wireframeEnabled, BlendMode blendMode) {
    ID3D11DeviceContext* context = graphicsDevice.GetContext();

    context->RSSetState(wireframeEnabled ? m_wireframeRasterizerState.Get() : m_solidRasterizerState.Get());

    context->OMSetDepthStencilState(m_depthEnabledState.Get(), 0);

    ID3D11BlendState* blendState = m_opaqueBlendState.Get();

    switch (blendMode) {
      case BlendMode::Opaque:
        blendState = m_opaqueBlendState.Get();
        break;

      case BlendMode::Alpha:
        blendState = m_alphaBlendState.Get();
        break;

      case BlendMode::Additive:
        blendState = m_additiveBlendState.Get();
        break;

      default:
        blendState = m_opaqueBlendState.Get();
        break;
    }

    const float blendFactor[4] = {0.0f, 0.0f, 0.0f, 0.0f};

    graphicsDevice.GetContext()->OMSetBlendState(blendState, blendFactor, 0xffffffff);
  }
}