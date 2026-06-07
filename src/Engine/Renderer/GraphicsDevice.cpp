// src/Engine/Renderer/GraphicsDevice.cpp
#include "Engine/Renderer/GraphicsDevice.h"

#include "Engine/Core/Log.h"
#include "Engine/Renderer/DxHelpers.h"

#include <Windows.h>
#include <dxgi1_3.h>
#include <dxgidebug.h>

namespace Engine {
  bool GraphicsDevice::Initialize(void* nativeWindowHandle, uint32_t width, uint32_t height) {
    HWND hwnd = static_cast<HWND>(nativeWindowHandle);

    DXGI_SWAP_CHAIN_DESC swapChainDesc = {};
    swapChainDesc.BufferCount = 1;
    swapChainDesc.BufferDesc.Width = width;
    swapChainDesc.BufferDesc.Height = height;
    swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    swapChainDesc.OutputWindow = hwnd;
    swapChainDesc.SampleDesc.Count = 1;
    swapChainDesc.SampleDesc.Quality = 0;
    swapChainDesc.Windowed = TRUE;
    swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

    UINT createDeviceFlags = 0;

#if defined(_DEBUG)
    createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

    D3D_FEATURE_LEVEL featureLevels[] = {D3D_FEATURE_LEVEL_11_0};

    D3D_FEATURE_LEVEL createdFeatureLevel = {};

    HRESULT hr = D3D11CreateDeviceAndSwapChain(nullptr,
                                               D3D_DRIVER_TYPE_HARDWARE,
                                               nullptr,
                                               createDeviceFlags,
                                               featureLevels,
                                               1,
                                               D3D11_SDK_VERSION,
                                               &swapChainDesc,
                                               m_swapChain.GetAddressOf(),
                                               m_device.GetAddressOf(),
                                               &createdFeatureLevel,
                                               m_context.GetAddressOf());

#if defined(_DEBUG)
    if (FAILED(hr) && (createDeviceFlags & D3D11_CREATE_DEVICE_DEBUG)) {
      LogWarning("D3D11 debug layer is unavailable. Retrying without debug layer.");

      createDeviceFlags &= ~D3D11_CREATE_DEVICE_DEBUG;

      hr = D3D11CreateDeviceAndSwapChain(nullptr,
                                         D3D_DRIVER_TYPE_HARDWARE,
                                         nullptr,
                                         createDeviceFlags,
                                         featureLevels,
                                         1,
                                         D3D11_SDK_VERSION,
                                         &swapChainDesc,
                                         m_swapChain.GetAddressOf(),
                                         m_device.GetAddressOf(),
                                         &createdFeatureLevel,
                                         m_context.GetAddressOf());
    }
#endif

    if (!DX_CHECK(hr)) {
      return false;
    }

    m_debugLayerEnabled = (createDeviceFlags & D3D11_CREATE_DEVICE_DEBUG) != 0;

#if defined(_DEBUG)
    Microsoft::WRL::ComPtr<ID3D11InfoQueue> infoQueue;

    if (SUCCEEDED(m_device.As(&infoQueue))) {
      infoQueue->SetBreakOnSeverity(D3D11_MESSAGE_SEVERITY_CORRUPTION, TRUE);
      infoQueue->SetBreakOnSeverity(D3D11_MESSAGE_SEVERITY_ERROR, TRUE);
    }
#endif

    Microsoft::WRL::ComPtr<ID3D11Texture2D> backBuffer;

    hr = m_swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(backBuffer.GetAddressOf()));

    if (!DX_CHECK(hr)) {
      return false;
    }

    hr = m_device->CreateRenderTargetView(backBuffer.Get(), nullptr, m_backBufferRTV.GetAddressOf());

    if (!DX_CHECK(hr)) {
      return false;
    }

    D3D11_TEXTURE2D_DESC depthDesc = {};
    depthDesc.Width = width;
    depthDesc.Height = height;
    depthDesc.ArraySize = 1;
    depthDesc.MipLevels = 1;
    depthDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
    depthDesc.SampleDesc.Count = 1;
    depthDesc.SampleDesc.Quality = 0;
    depthDesc.Usage = D3D11_USAGE_DEFAULT;
    depthDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
    depthDesc.CPUAccessFlags = 0;
    depthDesc.MiscFlags = 0;

    hr = m_device->CreateTexture2D(&depthDesc, nullptr, m_depthStencilBuffer.GetAddressOf());

    if (!DX_CHECK(hr)) {
      return false;
    }

    hr = m_device->CreateDepthStencilView(m_depthStencilBuffer.Get(), nullptr, m_depthStencilView.GetAddressOf());

    if (!DX_CHECK(hr)) {
      return false;
    }

    D3D11_VIEWPORT viewport = {};
    viewport.TopLeftX = 0.0f;
    viewport.TopLeftY = 0.0f;
    viewport.Width = static_cast<float>(width);
    viewport.Height = static_cast<float>(height);
    viewport.MinDepth = 0.0f;
    viewport.MaxDepth = 1.0f;

    m_context->RSSetViewports(1, &viewport);

    LogInfo("GraphicsDevice initialized.");

    return true;
  }

  void GraphicsDevice::Shutdown() {
    if (m_context) {
      m_context->ClearState();
      m_context->Flush();
    }

    m_depthStencilView.Reset();
    m_depthStencilBuffer.Reset();
    m_backBufferRTV.Reset();
    m_swapChain.Reset();
    m_context.Reset();
    m_device.Reset();

    ReportLiveObjects();
  }

  void GraphicsDevice::BeginFrame(float r, float g, float b, float a) {
    const float clearColor[] = {r, g, b, a};

    ID3D11RenderTargetView* renderTargets[] = {m_backBufferRTV.Get()};

    m_context->OMSetRenderTargets(1, renderTargets, m_depthStencilView.Get());

    m_context->ClearRenderTargetView(m_backBufferRTV.Get(), clearColor);

    m_context->ClearDepthStencilView(m_depthStencilView.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
  }

  void GraphicsDevice::EndFrame() { DX_CHECK(m_swapChain->Present(1, 0)); }

  void GraphicsDevice::ReportLiveObjects() {
#if defined(_DEBUG)
    if (!m_debugLayerEnabled) {
      return;
    }

    Microsoft::WRL::ComPtr<IDXGIDebug1> dxgiDebug;

    if (SUCCEEDED(DXGIGetDebugInterface1(0, IID_PPV_ARGS(&dxgiDebug)))) {
      dxgiDebug->ReportLiveObjects(
          DXGI_DEBUG_ALL, static_cast<DXGI_DEBUG_RLO_FLAGS>(DXGI_DEBUG_RLO_DETAIL | DXGI_DEBUG_RLO_IGNORE_INTERNAL));
    }
#endif
  }
}