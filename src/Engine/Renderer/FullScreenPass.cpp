#include "Engine/Renderer/FullscreenPass.h"

#include "Engine/Renderer/DxHelpers.h"
#include "Engine/Renderer/GraphicsDevice.h"
#include "Engine/Renderer/RenderTarget.h"

namespace Engine {
  bool FullscreenPass::Initialize(GraphicsDevice& graphicsDevice) {
    if (!m_shader.Initialize(graphicsDevice, L"assets/shaders/FullscreenCopy.hlsl", nullptr, 0)) {
      return false;
    }

    D3D11_SAMPLER_DESC samplerDesc = {};
    samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
    samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
    samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
    samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
    samplerDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
    samplerDesc.MinLOD = 0.0f;
    samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

    HRESULT hr = graphicsDevice.GetDevice()->CreateSamplerState(&samplerDesc, m_samplerState.GetAddressOf());

    return DX_CHECK(hr);
  }

  void FullscreenPass::Shutdown() {
    m_samplerState.Reset();
    m_shader.Shutdown();
  }

  void FullscreenPass::Render(GraphicsDevice& graphicsDevice, RenderTarget& sourceTexture) {
    ID3D11DeviceContext* context = graphicsDevice.GetContext();

    m_shader.Bind(graphicsDevice);

    ID3D11ShaderResourceView* shaderResources[] = {sourceTexture.GetShaderResourceView()};

    ID3D11SamplerState* samplers[] = {m_samplerState.Get()};

    context->PSSetShaderResources(0, 1, shaderResources);
    context->PSSetSamplers(0, 1, samplers);

    context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    context->Draw(3, 0);

    ID3D11ShaderResourceView* nullShaderResources[] = {nullptr};

    context->PSSetShaderResources(0, 1, nullShaderResources);
  }
}