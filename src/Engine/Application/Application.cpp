#include "Engine/Application/Application.h"

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

    if (!m_graphicsDevice.Initialize(m_window.GetNativeHandle(), m_window.GetWidth(), m_window.GetHeight())) {
      MessageBoxW(nullptr, L"Failed to initialize DirectX 11.", L"Error", MB_OK);
      return false;
    }

    if (!m_trianglePass.Initialize(m_graphicsDevice)) {
      MessageBoxW(nullptr, L"Failed to initialize triangle pass.", L"Error", MB_OK);
      return false;
    }

    m_isRunning = true;
    return true;
  }

  void Application::Shutdown() {
    m_trianglePass.Shutdown();
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

  void Application::Tick(float deltaSeconds) { (void)deltaSeconds; }

  void Application::Render() {
    m_graphicsDevice.BeginFrame(1.0f, 1.0f, 1.0f, 1.0f);

    m_trianglePass.Render(m_graphicsDevice, m_timer.GetTotalSeconds());

    m_graphicsDevice.EndFrame();
  }
}