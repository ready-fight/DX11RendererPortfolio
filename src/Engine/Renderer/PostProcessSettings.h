// src/Engine/Renderer/PostProcessSettings.h
#pragma once

namespace Engine {
  struct PostProcessSettings final {
    float grayscaleAmount = 0.0f;
    float exposure = 1.0f;
    float contrast = 1.0f;
    float vignetteAmount = 0.0f;

    float depthVisualizationAmount = 0.0f;
    float depthVisualizationRange = 20.0f;

    float bloomAmount = 1.0f;
    float bloomThreshold = 0.0f;
    float bloomRadius = 2.0f;

    float toneMappingAmount = 1.0f;
  };
}