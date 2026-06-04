#include "Engine/Application/Application.h"

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

    Engine::Application app;

    if (!app.Initialize())
    {
        return -1;
    }

    return app.Run();
}