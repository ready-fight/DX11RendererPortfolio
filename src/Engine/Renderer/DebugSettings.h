#pragma once

namespace Engine
{
    struct DebugSettings final
    {
        bool wireframeEnabled = false;
        bool grayscaleEnabled = false;
        bool vignetteEnabled = false;
        bool normalVisualizationEnabled = false;
        bool depthVisualizationEnabled = false;
    };
}