#pragma once

#include <cstdint>

namespace Engine {
  struct WindowDesc {
    const wchar_t *title = L"RenderSandbox";
    uint32_t width = 1280;
    uint32_t height = 720;
  };

  class Window {
  public:
    virtual ~Window() = default;

    virtual bool Create(const WindowDesc &desc) = 0;
    virtual void Destroy() = 0;
    virtual bool ProcessMessages() = 0;

    virtual void *GetNativeHandle() const = 0;

    virtual uint32_t GetWidth() const = 0;
    virtual uint32_t GetHeight() const = 0;
  };
}