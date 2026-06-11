// src/Engine/Platform/Win32Window.cpp
#include "Engine/Platform/Win32Window.h"
#include <imgui_impl_win32.h>

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);

namespace Engine {
  Win32Window::~Win32Window() { Destroy(); }

  bool Win32Window::Create(const WindowDesc& desc) {
    m_instance = GetModuleHandleW(nullptr);
    m_width = desc.width;
    m_height = desc.height;

    const wchar_t* className = L"DX11RendererPortfolioWindowClass";

    WNDCLASSEXW wc = {};
    wc.cbSize = sizeof(WNDCLASSEXW);
    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc = &Win32Window::StaticWindowProc;
    wc.hInstance = m_instance;
    wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wc.lpszClassName = className;

    RegisterClassExW(&wc);

    RECT rect = {};
    rect.right = static_cast<LONG>(m_width);
    rect.bottom = static_cast<LONG>(m_height);

    AdjustWindowRect(&rect, WS_OVERLAPPEDWINDOW, FALSE);

    m_hwnd = CreateWindowExW(0,
                             className,
                             desc.title,
                             WS_OVERLAPPEDWINDOW,
                             CW_USEDEFAULT,
                             CW_USEDEFAULT,
                             rect.right - rect.left,
                             rect.bottom - rect.top,
                             nullptr,
                             nullptr,
                             m_instance,
                             this);

    if (!m_hwnd) {
      return false;
    }

    ShowWindow(m_hwnd, SW_SHOW);
    UpdateWindow(m_hwnd);

    return true;
  }

  void Win32Window::Destroy() {
    if (m_hwnd) {
      DestroyWindow(m_hwnd);
      m_hwnd = nullptr;
    }
  }

  bool Win32Window::ProcessMessages() {
    MSG msg = {};

    while (PeekMessageW(&msg, nullptr, 0, 0, PM_REMOVE)) {
      if (msg.message == WM_QUIT) {
        return false;
      }

      TranslateMessage(&msg);
      DispatchMessageW(&msg);
    }

    return true;
  }

  LRESULT CALLBACK Win32Window::StaticWindowProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
    Win32Window* window = nullptr;

    if (message == WM_NCCREATE) {
      auto* createStruct = reinterpret_cast<CREATESTRUCTW*>(lParam);
      window = static_cast<Win32Window*>(createStruct->lpCreateParams);

      SetWindowLongPtrW(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(window));
    } else {
      window = reinterpret_cast<Win32Window*>(GetWindowLongPtrW(hwnd, GWLP_USERDATA));
    }

    if (window) {
      return window->WindowProc(hwnd, message, wParam, lParam);
    }

    return DefWindowProcW(hwnd, message, wParam, lParam);
  }

  LRESULT Win32Window::WindowProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
    if (ImGui_ImplWin32_WndProcHandler(hwnd, message, wParam, lParam)) {
      return true;
    }

    switch (message) {
      case WM_CLOSE:
        PostQuitMessage(0);
        return 0;

      case WM_DESTROY:
        PostQuitMessage(0);
        return 0;

      default:
        return DefWindowProcW(hwnd, message, wParam, lParam);
    }
  }
}