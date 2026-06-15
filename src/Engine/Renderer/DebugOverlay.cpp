#include "Engine/Renderer/DebugOverlay.h"
#include "Engine/Renderer/GraphicsDevice.h"
#include "Engine/Renderer/MaterialTypes.h"
#include "Engine/Renderer/RenderResourceManager.h"

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

  void DebugOverlay::Draw(DebugSettings& debugSettings, PostProcessSettings& postProcessSettings,
                          const RenderStats& renderStats, const RenderResourceManager& renderResources,
                          std::vector<SceneObject>& sceneObjects) {

    ImGui::Begin("Renderer Debug", nullptr, ImGuiWindowFlags_NoMove);

    ImGui::Text("Frame Stats");
    ImGui::Text("FPS: %.1f", renderStats.framesPerSecond);
    ImGui::Text("Frame Time: %.2f ms", renderStats.frameTimeMs);
    ImGui::Text("Draw Calls: %u", renderStats.drawCalls);
    ImGui::Text("Visible Objects: %u", renderStats.visibleObjects);

    ImGui::Separator();

    ImGui::Text("Render States");
    ImGui::Checkbox("Wireframe", &debugSettings.wireframeEnabled);

    ImGui::Separator();

    ImGui::Text("Post Effects");
    ImGui::Checkbox("Grayscale", &debugSettings.grayscaleEnabled);
    ImGui::Checkbox("Vignette", &debugSettings.vignetteEnabled);

    ImGui::SliderFloat("Exposure", &postProcessSettings.exposure, 0.1f, 3.0f);

    ImGui::SliderFloat("Contrast", &postProcessSettings.contrast, 0.1f, 3.0f);

    ImGui::Separator();

    ImGui::Text("Scene Objects");

    for (SceneObject& object : sceneObjects) {
      ImGui::PushID(object.name.c_str());

      if (ImGui::TreeNode(object.name.c_str())) {

        const MaterialHandle materialHandle = object.materialInstance.material;
        const MaterialShaderType shaderType = renderResources.GetMaterialShaderType(materialHandle);

        ImGui::Text("Current Material: %s", renderResources.GetMaterialDebugName(materialHandle));

        ImGui::Text("Shader Type: %s", ToString(shaderType));

        const uint32_t materialCount = renderResources.GetMaterialCount();

        const char* currentMaterialName = renderResources.GetMaterialDebugName(materialHandle);

        if (ImGui::BeginCombo("Material", currentMaterialName)) {
          for (uint32_t materialIndex = 0; materialIndex < materialCount; ++materialIndex) {
            MaterialHandle candidateHandle = renderResources.GetMaterialHandleAt(materialIndex);

            const bool isSelected = candidateHandle.value == object.materialInstance.material.value;

            const char* candidateName = renderResources.GetMaterialDebugName(candidateHandle);

            if (ImGui::Selectable(candidateName, isSelected)) {
              object.materialInstance.material = candidateHandle;
            }

            if (isSelected) {
              ImGui::SetItemDefaultFocus();
            }
          }

          ImGui::EndCombo();
        }

        ImGui::Checkbox("Enabled", &object.enabled);

        ImGui::SliderFloat("Rotation Speed", &object.rotationSpeed, 0.0f, 5.0f);

        ImGui::ColorEdit4("Base Color", &object.materialInstance.baseColor.x);

        ImGui::DragFloat3("Position", &object.transform.position.x, 0.05f);

        ImGui::DragFloat3("Scale", &object.transform.scale.x, 0.05f, 0.1f, 5.0f);

        ImGui::TreePop();
      }

      ImGui::PopID();
    }

    ImGui::End();
  }

  void DebugOverlay::EndFrame() {
    ImGui::Render();
    ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
  }
}