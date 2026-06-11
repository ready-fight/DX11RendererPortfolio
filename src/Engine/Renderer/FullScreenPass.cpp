#include "Engine/Renderer/FullscreenPass.h"

#include "Engine/Renderer/DxHelpers.h"
#include "Engine/Renderer/GraphicsDevice.h"
#include "Engine/Renderer/RenderTarget.h"

#include <DirectXMath.h>

namespace Engine {

  struct PostProcessConstants {
    DirectX::XMFLOAT4 settings;
  };

  static_assert(sizeof(PostProcessConstants) % 16 == 0);

  bool FullscreenPass::Initialize(GraphicsDevice& graphicsDevice) {
    if (!m_shader.Initialize(graphicsDevice, L"assets/shaders/FullscreenCopy.hlsl", nullptr, 0)) {
      return false;
    }

    if (!m_postProcessBuffer.CreateConstantBuffer(graphicsDevice, sizeof(PostProcessConstants))) {
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

  void FullscreenPass::Render(GraphicsDevice& graphicsDevice, RenderTarget& sourceTexture, bool grayscaleEnabled) {
    ID3D11DeviceContext* context = graphicsDevice.GetContext();

    PostProcessConstants constants = {};
    constants.settings =
        grayscaleEnabled ? DirectX::XMFLOAT4(1.0f, 0.0f, 0.0f, 0.0f) : DirectX::XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f);

    m_postProcessBuffer.Update(graphicsDevice, &constants, sizeof(constants));

    m_shader.Bind(graphicsDevice);
    m_postProcessBuffer.BindConstantBufferPS(graphicsDevice, 0);

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