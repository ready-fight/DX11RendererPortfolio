#include "Engine/Application/Application.h"
#include "Engine/Core/Log.h"

#include <Windows.h>

namespace Engine {
  bool Application::Initialize() {
    WindowDesc windowDesc = {};
    windowDesc.title = L"DX11 Renderer Portfolio";
    windowDesc.width = 1280;
    windowDesc.height = 720;

    if (!m_window.Create(windowDesc)) {
      MessageBoxW(nullptr, L"Failed to create window.", L"Error", MB_OK);
      return false;
    }

    const float aspectRatio = static_cast<float>(m_window.GetWidth()) / static_cast<float>(m_window.GetHeight());

    m_camera.SetTarget(0.0f, 0.0f, 0.0f);
    m_camera.SetOrbit(0.0f, 0.15f, 4.5f);
    m_camera.SetPerspective(DirectX::XMConvertToRadians(60.0f), aspectRatio, 0.1f, 100.0f);

    if (!m_graphicsDevice.Initialize(m_window.GetNativeHandle(), m_window.GetWidth(), m_window.GetHeight())) {
      MessageBoxW(nullptr, L"Failed to initialize DirectX 11.", L"Error", MB_OK);
      return false;
    }

    if (!m_sceneRenderTarget.InitializeColor(
            m_graphicsDevice, m_window.GetWidth(), m_window.GetHeight(), DXGI_FORMAT_R8G8B8A8_UNORM)) {
      MessageBoxW(nullptr, L"Failed to initialize scene render target.", L"Error", MB_OK);
      return false;
    }

    if (!m_fullscreenPass.Initialize(m_graphicsDevice)) {
      MessageBoxW(nullptr, L"Failed to initialize fullscreen pass.", L"Error", MB_OK);
      return false;
    }

    if (!m_renderStates.Initialize(m_graphicsDevice)) {
      return false;
    }

    if (!m_meshPass.Initialize(m_graphicsDevice)) {
      MessageBoxW(nullptr, L"Failed to initialize mesh pass.", L"Error", MB_OK);
      return false;
    }

    m_isRunning = true;
    return true;
  }

  void Application::Shutdown() {
    m_meshPass.Shutdown();
    m_fullscreenPass.Shutdown();
    m_sceneRenderTarget.Shutdown();
    m_renderStates.Shutdown();
    m_graphicsDevice.Shutdown();
    m_window.Destroy();
    m_isRunning = false;
  }

  int Application::Run() {
    while (m_isRunning) {
      if (!m_window.ProcessMessages()) {
        m_isRunning = false;
        break;
      }

      m_timer.Tick();

      Tick(m_timer.GetDeltaSeconds());
      Render();
    }

    Shutdown();
    return 0;
  }

  void Application::Tick(float deltaSeconds) {
    (void)deltaSeconds;
    const bool f1Down = (GetAsyncKeyState(VK_F1) & 0x8000) != 0;

    if (f1Down && !m_f1WasDown) {
      m_wireframeEnabled = !m_wireframeEnabled;

      if (m_wireframeEnabled) {
        LogInfo("Wireframe mode enabled.");
      } else {
        LogInfo("Wireframe mode disabled.");
      }
    }

    m_f1WasDown = f1Down;

    const bool f2Down = (GetAsyncKeyState(VK_F2) & 0x8000) != 0;

    if (f2Down && !m_f2WasDown) {
      m_grayscaleEnabled = !m_grayscaleEnabled;

      if (m_grayscaleEnabled) {
        LogInfo("Grayscale post effect enabled.");
      } else {
        LogInfo("Grayscale post effect disabled.");
      }
    }

    m_f2WasDown = f2Down;

    const float orbitSpeed = 1.5f * deltaSeconds;
    const float zoomSpeed = 3.0f * deltaSeconds;

    if (GetAsyncKeyState(VK_LEFT) & 0x8000) {
      m_camera.AddOrbit(-orbitSpeed, 0.0f);
    }

    if (GetAsyncKeyState(VK_RIGHT) & 0x8000) {
      m_camera.AddOrbit(orbitSpeed, 0.0f);
    }

    if (GetAsyncKeyState(VK_UP) & 0x8000) {
      m_camera.AddOrbit(0.0f, orbitSpeed);
    }

    if (GetAsyncKeyState(VK_DOWN) & 0x8000) {
      m_camera.AddOrbit(0.0f, -orbitSpeed);
    }

    if (GetAsyncKeyState('Q') & 0x8000) {
      m_camera.AddDistance(-zoomSpeed);
    }

    if (GetAsyncKeyState('E') & 0x8000) {
      m_camera.AddDistance(zoomSpeed);
    }
  }

  void Application::Render() {
    m_graphicsDevice.SetRenderTarget(m_sceneRenderTarget, &m_graphicsDevice.GetDepthStencilBuffer());

    m_graphicsDevice.ClearRenderTarget(m_sceneRenderTarget, 0.05f, 0.08f, 0.12f, 1.0f);

    m_graphicsDevice.ClearDepthStencil(m_graphicsDevice.GetDepthStencilBuffer());

    m_renderStates.Apply(m_graphicsDevice, m_wireframeEnabled);

    m_meshPass.Render(m_graphicsDevice, m_camera, m_timer.GetTotalSeconds());

    m_graphicsDevice.SetBackBufferRenderTarget();

    m_graphicsDevice.ClearRenderTarget(m_graphicsDevice.GetBackBufferRenderTarget(), 0.0f, 0.0f, 0.0f, 1.0f);

    m_renderStates.Apply(m_graphicsDevice, false);

    m_fullscreenPass.Render(m_graphicsDevice, m_sceneRenderTarget, m_grayscaleEnabled);

    m_graphicsDevice.EndFrame();
  }
}