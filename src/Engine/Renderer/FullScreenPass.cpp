#include "Engine/Renderer/FullscreenPass.h"

#include "Engine/Renderer/DebugSettings.h"
#include "Engine/Renderer/DepthStencilBuffer.h"
#include "Engine/Renderer/DxHelpers.h"
#include "Engine/Renderer/GraphicsDevice.h"
#include "Engine/Renderer/RenderTarget.h"

#include <DirectXMath.h>

namespace Engine {

  struct PostProcessConstants {
    DirectX::XMFLOAT4 settings;
    DirectX::XMFLOAT4 debugSettings;
    DirectX::XMFLOAT4 bloomSettings;
    DirectX::XMFLOAT4 textureSize;
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

  void FullscreenPass::Render(GraphicsDevice& graphicsDevice, RenderTarget& sourceTexture,
                              DepthStencilBuffer& sceneDepth, const PostProcessSettings& settings,
                              DebugSettings& debugSettings) {
    ID3D11DeviceContext* context = graphicsDevice.GetContext();

    PostProcessConstants constants = {};

    constants.settings = {settings.grayscaleAmount, settings.exposure, settings.contrast, settings.vignetteAmount};

    constants.debugSettings = {settings.depthVisualizationAmount, 0.1f, 100.0f, settings.depthVisualizationRange};

    constants.bloomSettings =
        debugSettings.bloomEnabled
            ? DirectX::XMFLOAT4(settings.bloomAmount, settings.bloomThreshold, settings.bloomRadius, 0.0f)
            : DirectX::XMFLOAT4(0.0f, settings.bloomThreshold, settings.bloomRadius, 0.0f);

    constants.textureSize = {
        static_cast<float>(sourceTexture.GetWidth()), static_cast<float>(sourceTexture.GetHeight()), 0.0f, 0.0f};

    m_postProcessBuffer.Update(graphicsDevice, &constants, sizeof(constants));

    m_shader.Bind(graphicsDevice);
    m_postProcessBuffer.BindConstantBufferPS(graphicsDevice, 0);

    ID3D11ShaderResourceView* shaderResources[] = {sourceTexture.GetShaderResourceView(),
                                                   sceneDepth.GetShaderResourceView()};

    ID3D11SamplerState* samplers[] = {m_samplerState.Get()};

    context->PSSetShaderResources(0, 2, shaderResources);
    context->PSSetSamplers(0, 1, samplers);

    context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    graphicsDevice.Draw(3, 0);

    ID3D11ShaderResourceView* nullShaderResources[] = {nullptr, nullptr};

    context->PSSetShaderResources(0, 2, nullShaderResources);
  }
}