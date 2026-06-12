#pragma once

#include <cstdint>

namespace Engine
{
    struct RenderStats final
    {
        float frameTimeMs = 0.0f;
        float framesPerSecond = 0.0f;

        uint32_t drawCalls = 0;
        uint32_t visibleObjects = 0;
    };
}