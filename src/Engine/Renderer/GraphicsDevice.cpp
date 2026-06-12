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

    if (!m_backBufferRenderTarget.InitializeFromBackBuffer(*this)) {
      return false;
    }

    if (!m_depthStencilBuffer.Initialize(*this, width, height)) {
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

  void GraphicsDevice::SetRenderTarget(RenderTarget& renderTarget, DepthStencilBuffer* depthStencilBuffer) {
    ID3D11RenderTargetView* renderTargets[] = {renderTarget.GetRenderTargetView()};

    m_context->OMSetRenderTargets(
        1, renderTargets, depthStencilBuffer ? depthStencilBuffer->GetDepthStencilView() : nullptr);
  }

  void GraphicsDevice::SetBackBufferRenderTarget() { SetRenderTarget(m_backBufferRenderTarget, nullptr); }

  void GraphicsDevice::ClearRenderTarget(RenderTarget& renderTarget, float r, float g, float b, float a) {
    const float clearColor[] = {r, g, b, a};

    m_context->ClearRenderTargetView(renderTarget.GetRenderTargetView(), clearColor);
  }

  void GraphicsDevice::ClearDepthStencil(DepthStencilBuffer& depthStencilBuffer) {
    m_context->ClearDepthStencilView(
        depthStencilBuffer.GetDepthStencilView(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
  }

  void GraphicsDevice::Shutdown() {
    if (m_context) {
      m_context->ClearState();
      m_context->Flush();
    }

    m_depthStencilBuffer.Shutdown();
    m_backBufferRenderTarget.Shutdown();

    m_swapChain.Reset();
    m_context.Reset();
    m_device.Reset();

    ReportLiveObjects();
  }

  void GraphicsDevice::BeginFrame(float r, float g, float b, float a) {
    SetRenderTarget(m_backBufferRenderTarget, &m_depthStencilBuffer);
    ClearRenderTarget(m_backBufferRenderTarget, r, g, b, a);
    ClearDepthStencil(m_depthStencilBuffer);
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

  void GraphicsDevice::BeginFrameStats(float deltaSeconds) {
    m_renderStats.frameTimeMs = deltaSeconds * 1000.0f;
    m_renderStats.drawCalls = 0;
    m_renderStats.visibleObjects = 0;

    m_fpsAccumulatedTime += deltaSeconds;
    ++m_fpsFrameCount;

    if (m_fpsAccumulatedTime >= 0.25f) {
      m_renderStats.framesPerSecond = static_cast<float>(m_fpsFrameCount) / m_fpsAccumulatedTime;

      m_fpsAccumulatedTime = 0.0f;
      m_fpsFrameCount = 0;
    }
  }

  void GraphicsDevice::Draw(uint32_t vertexCount, uint32_t startVertexLocation) {
    m_context->Draw(vertexCount, startVertexLocation);
    ++m_renderStats.drawCalls;
  }

  void GraphicsDevice::DrawIndexed(uint32_t indexCount, uint32_t startIndexLocation, int32_t baseVertexLocation) {
    m_context->DrawIndexed(indexCount, startIndexLocation, baseVertexLocation);

    ++m_renderStats.drawCalls;
  }

  void GraphicsDevice::AddVisibleObject() { ++m_renderStats.visibleObjects; }
}