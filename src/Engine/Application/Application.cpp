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

    m_camera.SetPosition(0.0f, 0.0f, -2.5f);
    m_camera.SetTarget(0.0f, 0.0f, 0.0f);
    m_camera.SetPerspective(DirectX::XMConvertToRadians(60.0f), aspectRatio, 0.1f, 100.0f);

    if (!m_graphicsDevice.Initialize(m_window.GetNativeHandle(), m_window.GetWidth(), m_window.GetHeight())) {
      MessageBoxW(nullptr, L"Failed to initialize DirectX 11.", L"Error", MB_OK);
      return false;
    }

    if (!m_renderStates.Initialize(m_graphicsDevice)) {
      return false;
    }

    if (!m_meshPass.Initialize(m_graphicsDevice)) {
      MessageBoxW(nullptr, L"Failed to initialize triangle pass.", L"Error", MB_OK);
      return false;
    }

    m_isRunning = true;
    return true;
  }

  void Application::Shutdown() {
    m_meshPass.Shutdown();
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
  }

  void Application::Render() {

    m_graphicsDevice.BeginFrame(0.0f, 0.0f, 0.0f, 0.0f);

    m_renderStates.Apply(m_graphicsDevice, m_wireframeEnabled);

    m_meshPass.Render(m_graphicsDevice, m_camera, m_timer.GetTotalSeconds());

    m_graphicsDevice.EndFrame();
  }
}