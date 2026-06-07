#pragma once

#include "Engine/Core/Timer.h"
#include "Engine/Platform/Win32Window.h"
#include "Engine/Renderer/GraphicsDevice.h"
#include "Engine/Renderer/TrianglePass.h"
#include "Engine/Renderer/Camera.h"

namespace Engine {
  class Application final {
  public:
    bool Initialize();
    void Shutdown();

    int Run();

  private:
    void Tick(float deltaSeconds);
    void Render();

  private:
    Win32Window m_window;
    GraphicsDevice m_graphicsDevice;
    TrianglePass m_trianglePass;
    Camera m_camera;
    Timer m_timer;

    bool m_isRunning = false;
  };
}