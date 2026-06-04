#pragma once

#include <Windows.h>

namespace Engine
{
    bool CheckHRESULT(HRESULT hr, const char* expression, const char* file, int line);

    #define DX_CHECK(expression) \
    ::Engine::CheckHRESULT((expression), #expression, __FILE__, __LINE__)
}