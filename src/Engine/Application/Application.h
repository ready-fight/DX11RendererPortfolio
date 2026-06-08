#pragma once

#include "Engine/Core/Timer.h"
#include "Engine/Platform/Win32Window.h"
#include "Engine/Renderer/Camera.h"
#include "Engine/Renderer/GraphicsDevice.h"
#include "Engine/Renderer/MeshPass.h"
#include "Engine/Renderer/RenderStates.h"


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
    RenderStates m_renderStates;
    MeshPass m_meshPass;
    Camera m_camera;
    Timer m_timer;

    bool m_isRunning = false;
    bool m_wireframeEnabled = false;
    bool m_f1WasDown = false;
  };
}