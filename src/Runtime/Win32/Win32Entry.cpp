// src/Runtime/Win32/Win32Entry.cpp

#include "Engine/Platform/Win32Window.h"
#include "Engine/Renderer/GraphicsDevice.h"

#include <Windows.h>

int WINAPI wWinMain(
    HINSTANCE instance,
    HINSTANCE previousInstance,
    PWSTR commandLine,
    int showCommand)
{
    (void)instance;
    (void)previousInstance;
    (void)commandLine;
    (void)showCommand;

    Engine::Win32Window window;

    Engine::WindowDesc windowDesc = {};
    windowDesc.title = L"DX11 Renderer Portfolio";
    windowDesc.width = 1280;
    windowDesc.height = 720;

    if (!window.Create(windowDesc))
    {
        MessageBoxW(nullptr, L"Failed to create window.", L"Error", MB_OK);
        return -1;
    }

    Engine::GraphicsDevice graphicsDevice;

    if (!graphicsDevice.Initialize(
        window.GetNativeHandle(),
        window.GetWidth(),
        window.GetHeight()))
    {
        MessageBoxW(nullptr, L"Failed to initialize DirectX 11.", L"Error", MB_OK);
        return -1;
    }

    while (window.ProcessMessages())
    {
        graphicsDevice.BeginFrame(0.05f, 0.08f, 0.12f, 1.0f);
        graphicsDevice.EndFrame();
    }

    graphicsDevice.Shutdown();
    window.Destroy();

    return 0;
}