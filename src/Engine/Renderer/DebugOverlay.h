#pragma once

#include "Engine/Renderer/DebugSettings.h"
#include "Engine/Renderer/PostProcessSettings.h"
#include "Engine/Renderer/RenderStats.h"

#include "Engine/Scene/SceneObject.h"

#include <vector>

namespace Engine {
  class GraphicsDevice;

  class DebugOverlay final {
  public:
    bool Initialize(void* nativeWindowHandle, GraphicsDevice& graphicsDevice);
    void Shutdown();

    void BeginFrame();
    void Draw(DebugSettings& debugSettings, PostProcessSettings& postProcessSettings, const RenderStats& renderStats,
              std::vector<SceneObject>& sceneObjects);
    void EndFrame();
  };
}