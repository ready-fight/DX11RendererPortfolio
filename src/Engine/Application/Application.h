#pragma once

#include "Engine/Core/Timer.h"
#include "Engine/Platform/Win32Window.h"
#include "Engine/Renderer/Camera.h"
#include "Engine/Renderer/FullscreenPass.h"
#include "Engine/Renderer/GraphicsDevice.h"
#include "Engine/Renderer/MeshPass.h"
#include "Engine/Renderer/RenderStates.h"
#include "Engine/Renderer/RenderTarget.h"

#include "Engine/Input/KeyboardInput.h"
#include "Engine/Renderer/DebugSettings.h"

#include "Engine/Renderer/DebugOverlay.h"
#include "Engine/Scene/Scene.h"
#include "Engine/Renderer/RenderResourceManager.h"

namespace Engine {
  class Application final {
  public:
    bool Initialize();
    void Shutdown();

    int Run();

  private:
    void Tick(float deltaSeconds);
    void Render();
    void BuildTestScene();

  private:
    Win32Window m_window;
    GraphicsDevice m_graphicsDevice;
    RenderStates m_renderStates;
    RenderResourceManager m_renderResources;

    RenderTarget m_sceneRenderTarget;
    FullscreenPass m_fullscreenPass;
    DebugOverlay m_debugOverlay;
    PostProcessSettings m_postProcessSettings;
    KeyboardInput m_keyboardInput;
    DebugSettings m_debugSettings;

    MeshPass m_meshPass;
    Scene m_scene;
    Camera m_camera;
    Timer m_timer;

    bool m_isRunning = false;
  };
}