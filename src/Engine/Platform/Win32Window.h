// src/Engine/Platform/Win32Window.h
#pragma once

#include "Engine/Platform/Window.h"

#include <Windows.h>

namespace Engine {
  class Win32Window final : public Window {
  public:
    Win32Window() = default;
    ~Win32Window() override;

    bool Create(const WindowDesc &desc) override;
    void Destroy() override;
    bool ProcessMessages() override;

    void *GetNativeHandle() const override { return m_hwnd; }

    uint32_t GetWidth() const override { return m_width; }
    uint32_t GetHeight() const override { return m_height; }

  private:
    static LRESULT CALLBACK StaticWindowProc(HWND hwnd, UINT message,
                                             WPARAM wParam, LPARAM lParam);
    LRESULT WindowProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);

  private:
    HWND m_hwnd = nullptr;
    HINSTANCE m_instance = nullptr;

    uint32_t m_width = 0;
    uint32_t m_height = 0;
  };
}