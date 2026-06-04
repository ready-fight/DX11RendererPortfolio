#include "Engine/Core/Log.h"

#include <Windows.h>

namespace Engine {
  namespace {
    void OutputLog(const char *prefix, const char *message) {
      OutputDebugStringA(prefix);
      OutputDebugStringA(message);
      OutputDebugStringA("\n");
    }
  }

  void LogInfo(const char *message) { OutputLog("[Info] ", message); }

  void LogWarning(const char *message) { OutputLog("[Warning] ", message); }

  void LogError(const char *message) { OutputLog("[Error] ", message); }
}