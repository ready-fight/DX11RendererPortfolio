#pragma once

#include <d3d11.h>
#include <dxgi.h>
#include <wrl/client.h>

#include <cstdint>

#include "Engine/Renderer/DepthStencilBuffer.h"
#include "Engine/Renderer/RenderStats.h"
#include "Engine/Renderer/RenderTarget.h"

namespace Engine {
  class GraphicsDevice final {
  public:
    bool Initialize(void* nativeWindowHandle, uint32_t width, uint32_t height);
    void Shutdown();

    void BeginFrame(float r, float g, float b, float a);
    void EndFrame();

    ID3D11Device* GetDevice() const { return m_device.Get(); }
    ID3D11DeviceContext* GetContext() const { return m_context.Get(); }
    IDXGISwapChain* GetSwapChain() const { return m_swapChain.Get(); }

    RenderTarget& GetBackBufferRenderTarget() { return m_backBufferRenderTarget; }
    DepthStencilBuffer& GetDepthStencilBuffer() { return m_depthStencilBuffer; }

    void SetRenderTarget(RenderTarget& renderTarget, DepthStencilBuffer* depthStencilBuffer);
    void SetBackBufferRenderTarget();

    void ClearRenderTarget(RenderTarget& renderTarget, float r, float g, float b, float a);
    void ClearDepthStencil(DepthStencilBuffer& depthStencilBuffer);

    void BeginFrameStats(float deltaSeconds);

    void Draw(uint32_t vertexCount, uint32_t startVertexLocation);
    void DrawIndexed(uint32_t indexCount, uint32_t startIndexLocation, int32_t baseVertexLocation);

    void AddVisibleObject();

    const RenderStats& GetRenderStats() const { return m_renderStats; }

    void BeginRenderPass(RenderPassType renderPassType);

    float GetFPS() { return m_fps; }

  private:
    void ReportLiveObjects();
    void RecordDrawCall();

  private:
    Microsoft::WRL::ComPtr<ID3D11Device> m_device;
    Microsoft::WRL::ComPtr<ID3D11DeviceContext> m_context;
    Microsoft::WRL::ComPtr<IDXGISwapChain> m_swapChain;

    RenderTarget m_backBufferRenderTarget;
    DepthStencilBuffer m_depthStencilBuffer;

    float m_fpsAccumulatedTime = 0.0f;
    uint32_t m_fpsFrameCount = 0;
    float m_fps = 0;

    bool m_debugLayerEnabled = false;

    RenderStats m_renderStats;
    RenderPassType m_activeRenderPass = RenderPassType::Scene;
    
  };
}