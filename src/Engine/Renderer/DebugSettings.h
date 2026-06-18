#pragma once

namespace Engine {
  enum class DebugViewMode { None, Normals, Depth };

  inline const char* ToString(DebugViewMode mode) {
    switch (mode) {
      case DebugViewMode::None:
        return "None";

      case DebugViewMode::Normals:
        return "Normals";

      case DebugViewMode::Depth:
        return "Depth";

      default:
        return "Unknown";
    }
  }

  struct DebugSettings final {
    bool wireframeEnabled = false;
    bool grayscaleEnabled = false;
    bool vignetteEnabled = false;

    DebugViewMode debugViewMode = DebugViewMode::None;
  };
}