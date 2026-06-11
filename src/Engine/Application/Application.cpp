#include "Engine/Application/Application.h"
#include "Engine/Core/Log.h"
#include "Engine/Renderer/PostProcessSettings.h"

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

    if (!m_debugOverlay.Initialize(m_window.GetNativeHandle(), m_graphicsDevice)) {
      MessageBoxW(nullptr, L"Failed to initialize debug overlay.", L"Error", MB_OK);
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

    m_postProcessSettings.grayscaleAmount = 0.0f;
    m_postProcessSettings.exposure = 1.0f;
    m_postProcessSettings.contrast = 1.0f;
    m_postProcessSettings.vignetteAmount = 0.0f;

    return true;
  }

  void Application::Shutdown() {
    m_meshPass.Shutdown();
    m_debugOverlay.Shutdown();
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

    m_keyboardInput.Update();

    if (m_keyboardInput.WasPressed(VK_F1)) {
      m_debugSettings.wireframeEnabled = !m_debugSettings.wireframeEnabled;

      LogInfo(m_debugSettings.wireframeEnabled ? "Wireframe mode enabled." : "Wireframe mode disabled.");
    }

    if (m_keyboardInput.WasPressed(VK_F2)) {
      m_debugSettings.grayscaleEnabled = !m_debugSettings.grayscaleEnabled;

      LogInfo(m_debugSettings.grayscaleEnabled ? "Grayscale post effect enabled." : "Grayscale post effect disabled.");
    }

    if (m_keyboardInput.WasPressed(VK_F3)) {
      m_debugSettings.vignetteEnabled = !m_debugSettings.vignetteEnabled;

      LogInfo(m_debugSettings.vignetteEnabled ? "Vignette post effect enabled." : "Vignette post effect disabled.");
    }

    const float orbitSpeed = 1.5f * deltaSeconds;
    const float zoomSpeed = 3.0f * deltaSeconds;

    if (m_keyboardInput.IsDown(VK_LEFT)) {
      m_camera.AddOrbit(-orbitSpeed, 0.0f);
    }

    if (m_keyboardInput.IsDown(VK_RIGHT)) {
      m_camera.AddOrbit(orbitSpeed, 0.0f);
    }

    if (m_keyboardInput.IsDown(VK_UP)) {
      m_camera.AddOrbit(0.0f, orbitSpeed);
    }

    if (m_keyboardInput.IsDown(VK_DOWN)) {
      m_camera.AddOrbit(0.0f, -orbitSpeed);
    }

    if (m_keyboardInput.IsDown('Q')) {
      m_camera.AddDistance(-zoomSpeed);
    }

    if (m_keyboardInput.IsDown('E')) {
      m_camera.AddDistance(zoomSpeed);
    }

    m_postProcessSettings.grayscaleAmount = m_debugSettings.grayscaleEnabled ? 1.0f : 0.0f;

    m_postProcessSettings.vignetteAmount = m_debugSettings.vignetteEnabled ? 1.0f : 0.0f;
  }

  void Application::Render() {
    m_graphicsDevice.SetRenderTarget(m_sceneRenderTarget, &m_graphicsDevice.GetDepthStencilBuffer());

    m_graphicsDevice.ClearRenderTarget(m_sceneRenderTarget, 0.05f, 0.08f, 0.12f, 1.0f);

    m_graphicsDevice.ClearDepthStencil(m_graphicsDevice.GetDepthStencilBuffer());

    m_renderStates.Apply(m_graphicsDevice, m_debugSettings.wireframeEnabled);

    m_meshPass.Render(m_graphicsDevice, m_camera, m_timer.GetTotalSeconds());

    m_graphicsDevice.SetBackBufferRenderTarget();

    m_graphicsDevice.ClearRenderTarget(m_graphicsDevice.GetBackBufferRenderTarget(), 0.0f, 0.0f, 0.0f, 1.0f);

    m_renderStates.Apply(m_graphicsDevice, false);

    m_fullscreenPass.Render(m_graphicsDevice, m_sceneRenderTarget, m_postProcessSettings);

    m_debugOverlay.BeginFrame();

    m_debugOverlay.Draw(m_debugSettings, m_postProcessSettings);

    m_debugOverlay.EndFrame();

    m_graphicsDevice.EndFrame();
  }
}