#include "Engine/Renderer/DebugOverlay.h"

#include "Engine/Renderer/GraphicsDevice.h"

#include <Windows.h>

#include <imgui.h>
#include <imgui_impl_dx11.h>
#include <imgui_impl_win32.h>

namespace Engine {
  bool DebugOverlay::Initialize(void* nativeWindowHandle, GraphicsDevice& graphicsDevice) {
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();

    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

    ImGui::StyleColorsDark();

    HWND hwnd = static_cast<HWND>(nativeWindowHandle);

    if (!ImGui_ImplWin32_Init(hwnd)) {
      return false;
    }

    if (!ImGui_ImplDX11_Init(graphicsDevice.GetDevice(), graphicsDevice.GetContext())) {
      return false;
    }

    return true;
  }

  void DebugOverlay::Shutdown() {
    ImGui_ImplDX11_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();
  }

  void DebugOverlay::BeginFrame() {
    ImGui_ImplDX11_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();
  }

  void DebugOverlay::Draw(DebugSettings& debugSettings, PostProcessSettings& postProcessSettings) {

    ImGui::SetNextWindowPos(ImVec2(ImGui::GetIO().DisplaySize.x, 0.0f), ImGuiCond_Always, ImVec2(1.0f, 0.0f));

    ImGui::Begin("Renderer Debug", nullptr, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoMove);

    ImGui::Text("Render States");
    ImGui::Checkbox("Wireframe", &debugSettings.wireframeEnabled);

    ImGui::Separator();

    ImGui::Text("Post Effects");
    ImGui::Checkbox("Grayscale", &debugSettings.grayscaleEnabled);
    ImGui::Checkbox("Vignette", &debugSettings.vignetteEnabled);

    ImGui::SliderFloat("Exposure", &postProcessSettings.exposure, 0.1f, 3.0f);

    ImGui::SliderFloat("Contrast", &postProcessSettings.contrast, 0.1f, 3.0f);

    ImGui::End();
  }

  void DebugOverlay::EndFrame() {
    ImGui::Render();
    ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
  }
}