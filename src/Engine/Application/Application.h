#pragma once

#include "Engine/Core/Timer.h"
#include "Engine/Platform/Win32Window.h"
#include "Engine/Renderer/GraphicsDevice.h"

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
    Timer m_timer;

    bool m_isRunning = false;
  };
}