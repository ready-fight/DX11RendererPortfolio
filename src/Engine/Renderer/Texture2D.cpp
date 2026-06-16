#include "Engine/Renderer/Texture2D.h"

#include "Engine/Core/Log.h"
#include "Engine/Renderer/DxHelpers.h"
#include "Engine/Renderer/GraphicsDevice.h"

#include <vector>
#include <wincodec.h>

namespace Engine {

  bool Texture2D::LoadFromFile(GraphicsDevice& graphicsDevice, const wchar_t* filePath) {
    Microsoft::WRL::ComPtr<IWICImagingFactory> imagingFactory;

    HRESULT hr = CoCreateInstance(
        CLSID_WICImagingFactory, nullptr, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(imagingFactory.GetAddressOf()));

    if (!DX_CHECK(hr)) {
      return false;
    }

    Microsoft::WRL::ComPtr<IWICBitmapDecoder> decoder;

    hr = imagingFactory->CreateDecoderFromFilename(
        filePath, nullptr, GENERIC_READ, WICDecodeMetadataCacheOnLoad, decoder.GetAddressOf());

    if (!DX_CHECK(hr)) {
      return false;
    }

    Microsoft::WRL::ComPtr<IWICBitmapFrameDecode> frame;

    hr = decoder->GetFrame(0, frame.GetAddressOf());

    if (!DX_CHECK(hr)) {
      return false;
    }

    Microsoft::WRL::ComPtr<IWICFormatConverter> converter;

    hr = imagingFactory->CreateFormatConverter(converter.GetAddressOf());

    if (!DX_CHECK(hr)) {
      return false;
    }

    hr = converter->Initialize(
        frame.Get(), GUID_WICPixelFormat32bppRGBA, WICBitmapDitherTypeNone, nullptr, 0.0, WICBitmapPaletteTypeCustom);

    if (!DX_CHECK(hr)) {
      return false;
    }

    UINT width = 0;
    UINT height = 0;

    hr = converter->GetSize(&width, &height);

    if (!DX_CHECK(hr)) {
      return false;
    }

    m_width = width;
    m_height = height;

    std::vector<uint8_t> pixels;
    pixels.resize(static_cast<size_t>(width) * static_cast<size_t>(height) * 4);

    const UINT stride = width * 4;
    const UINT imageSize = stride * height;

    hr = converter->CopyPixels(nullptr, stride, imageSize, pixels.data());

    if (!DX_CHECK(hr)) {
      return false;
    }

    D3D11_TEXTURE2D_DESC textureDesc = {};
    textureDesc.Width = width;
    textureDesc.Height = height;
    textureDesc.MipLevels = 1;
    textureDesc.ArraySize = 1;
    textureDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    textureDesc.SampleDesc.Count = 1;
    textureDesc.SampleDesc.Quality = 0;
    textureDesc.Usage = D3D11_USAGE_DEFAULT;
    textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;

    D3D11_SUBRESOURCE_DATA initialData = {};
    initialData.pSysMem = pixels.data();
    initialData.SysMemPitch = stride;

    hr = graphicsDevice.GetDevice()->CreateTexture2D(&textureDesc, &initialData, m_texture.GetAddressOf());

    if (!DX_CHECK(hr)) {
      return false;
    }

    hr = graphicsDevice.GetDevice()->CreateShaderResourceView(
        m_texture.Get(), nullptr, m_shaderResourceView.GetAddressOf());

    if (!DX_CHECK(hr)) {
      return false;
    }

    D3D11_SAMPLER_DESC samplerDesc = {};
    samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
    samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
    samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
    samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
    samplerDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
    samplerDesc.MinLOD = 0.0f;
    samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

    hr = graphicsDevice.GetDevice()->CreateSamplerState(&samplerDesc, m_samplerState.GetAddressOf());

    if (!DX_CHECK(hr)) {
      return false;
    }

    LogInfo("Texture loaded from file.");
    return true;
  }

  bool Texture2D::CreateCheckerboard(GraphicsDevice& graphicsDevice, uint32_t width, uint32_t height) {

    m_width = width;
    m_height = height;

    std::vector<uint32_t> pixels;
    pixels.resize(static_cast<size_t>(width) * static_cast<size_t>(height));

    constexpr uint32_t lightColor = 0xFFFFFFFF;
    constexpr uint32_t darkColor = 0xFF303030;
    constexpr uint32_t checkerSize = 8;

    for (uint32_t y = 0; y < height; ++y) {
      for (uint32_t x = 0; x < width; ++x) {
        const bool checker = ((x / checkerSize) + (y / checkerSize)) % 2 == 0;

        pixels[static_cast<size_t>(y) * width + x] = checker ? lightColor : darkColor;
      }
    }

    D3D11_TEXTURE2D_DESC textureDesc = {};
    textureDesc.Width = width;
    textureDesc.Height = height;
    textureDesc.MipLevels = 1;
    textureDesc.ArraySize = 1;
    textureDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    textureDesc.SampleDesc.Count = 1;
    textureDesc.Usage = D3D11_USAGE_DEFAULT;
    textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;

    D3D11_SUBRESOURCE_DATA initialData = {};
    initialData.pSysMem = pixels.data();
    initialData.SysMemPitch = width * sizeof(uint32_t);

    HRESULT hr = graphicsDevice.GetDevice()->CreateTexture2D(&textureDesc, &initialData, m_texture.GetAddressOf());

    if (!DX_CHECK(hr)) {
      return false;
    }

    hr = graphicsDevice.GetDevice()->CreateShaderResourceView(
        m_texture.Get(), nullptr, m_shaderResourceView.GetAddressOf());

    if (!DX_CHECK(hr)) {
      return false;
    }

    D3D11_SAMPLER_DESC samplerDesc = {};
    samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
    samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
    samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
    samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
    samplerDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
    samplerDesc.MinLOD = 0.0f;
    samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

    hr = graphicsDevice.GetDevice()->CreateSamplerState(&samplerDesc, m_samplerState.GetAddressOf());

    if (!DX_CHECK(hr)) {
      return false;
    }

    LogInfo("Checkerboard texture created.");
    return true;
  }

  void Texture2D::Shutdown() {
    m_samplerState.Reset();
    m_shaderResourceView.Reset();
    m_texture.Reset();

    m_width = 0;
    m_height = 0;
    m_debugName = "Unnamed Texture";
  }

  void Texture2D::BindPS(GraphicsDevice& graphicsDevice, uint32_t textureSlot, uint32_t samplerSlot) {
    ID3D11ShaderResourceView* shaderResources[] = {m_shaderResourceView.Get()};

    ID3D11SamplerState* samplers[] = {m_samplerState.Get()};

    graphicsDevice.GetContext()->PSSetShaderResources(textureSlot, 1, shaderResources);

    graphicsDevice.GetContext()->PSSetSamplers(samplerSlot, 1, samplers);
  }
}