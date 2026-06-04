#include "Engine/Renderer/DxHelpers.h"

#include "Engine/Core/Log.h"

#include <cstdio>

namespace Engine {
  bool CheckHRESULT(HRESULT hr, const char *expression, const char *file,
                    int line) {
    if (SUCCEEDED(hr)) {
      return true;
    }

    char buffer[1024] = {};

    std::snprintf(buffer, sizeof(buffer),
                  "HRESULT failed: 0x%08X\nExpression: %s\nFile: %s\nLine: %d",
                  static_cast<unsigned int>(hr), expression, file, line);

    LogError(buffer);
    return false;
  }
}