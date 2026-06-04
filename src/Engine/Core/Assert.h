#pragma once

#include "Engine/Core/Log.h"

#if defined(_MSC_VER)
    #define ENGINE_DEBUG_BREAK() __debugbreak()
#else
    #define ENGINE_DEBUG_BREAK()
#endif

#if defined(_DEBUG)
    #define ENGINE_ASSERT(condition, message) \
        do \
        { \
            if (!(condition)) \
            { \
                ::Engine::LogError(message); \
                ENGINE_DEBUG_BREAK(); \
            } \
        } while (false)
#else
    #define ENGINE_ASSERT(condition, message) ((void)0)
#endif