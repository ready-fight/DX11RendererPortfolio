#pragma once

#include <cstdint>

namespace Engine {
  struct MeshHandle final {
    static constexpr uint32_t Invalid = 0xFFFFFFFFu;

    uint32_t value = Invalid;

    bool IsValid() const { return value != Invalid; }
  };

  struct MaterialHandle final {
    static constexpr uint32_t Invalid = 0xFFFFFFFFu;

    uint32_t value = Invalid;

    bool IsValid() const { return value != Invalid; }
  };

  struct TextureHandle final {
    static constexpr uint32_t Invalid = 0xFFFFFFFFu;

    uint32_t value = Invalid;

    bool IsValid() const { return value != Invalid; }
  };
}