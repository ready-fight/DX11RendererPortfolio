#pragma once

#include "Engine/Renderer/DebugSettings.h"
#include "Engine/Renderer/PostProcessSettings.h"

namespace Engine
{
    class GraphicsDevice;

    class DebugOverlay final
    {
    public:
        bool Initialize(void* nativeWindowHandle, GraphicsDevice& graphicsDevice);
        void Shutdown();

        void BeginFrame();
        void Draw(DebugSettings& debugSettings, PostProcessSettings& postProcessSettings);
        void EndFrame();
    };
}