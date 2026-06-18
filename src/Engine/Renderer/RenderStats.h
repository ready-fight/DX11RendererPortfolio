#pragma once

#include <array>
#include <cstdint>

namespace Engine {

  enum class RenderPassType : uint32_t { Scene = 0, PostProcess, Count };

  constexpr uint32_t RenderPassCount = static_cast<uint32_t>(RenderPassType::Count);

  struct RenderPassStats final {
    const char* name = "Unknown";
    uint32_t drawCalls = 0;
  };

  struct RenderStats final {
    float frameTimeMs = 0.0f;

    uint32_t drawCalls = 0;
    uint32_t visibleObjects = 0;

    std::array<RenderPassStats, RenderPassCount> renderPasses = {{{"Scene", 0}, {"Post Process", 0}}};
  };

}