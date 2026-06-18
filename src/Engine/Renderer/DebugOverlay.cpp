#include "Engine/Renderer/DebugOverlay.h"
#include "Engine/Renderer/GraphicsDevice.h"
#include "Engine/Renderer/MaterialTypes.h"
#include "Engine/Renderer/RenderResourceManager.h"
#include "Engine/Scene/DirectionalLight.h"
#include "Engine/Scene/Scene.h"

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
                          const RenderStats& renderStats, const RenderResourceManager& renderResources, Scene& scene,
                          float FPS) {

    ImGui::Begin("Renderer Debug", nullptr, ImGuiWindowFlags_NoMove);

    if (ImGui::CollapsingHeader("Render Stats", ImGuiTreeNodeFlags_DefaultOpen)) {
      ImGui::Text("FPS: %.1f", FPS);
      ImGui::Text("Frame Time: %.3f ms", renderStats.frameTimeMs);
      ImGui::Text("Total Draw Calls: %u", renderStats.drawCalls);
      ImGui::Text("Visible Objects: %u", renderStats.visibleObjects);

      ImGui::SeparatorText("Render Passes");

      for (const RenderPassStats& passStats : renderStats.renderPasses) {
        ImGui::Text("%s: %u draw calls", passStats.name, passStats.drawCalls);
      }
    }

    ImGui::Separator();

    ImGui::Text("Render States");
    ImGui::Checkbox("Wireframe (F1)", &debugSettings.wireframeEnabled);

    ImGui::Separator();

    ImGui::Text("Post Effects");
    ImGui::Checkbox("Grayscale (F2)", &debugSettings.grayscaleEnabled);
    ImGui::Checkbox("Vignette (F3)", &debugSettings.vignetteEnabled);
    ImGui::Checkbox("Bloom (F6)", &debugSettings.bloomEnabled);

    ImGui::SliderFloat("Bloom Amount", &postProcessSettings.bloomAmount, 0.0f, 3.0f);
    ImGui::SliderFloat("Bloom Threshold", &postProcessSettings.bloomThreshold, 0.0f, 2.0f);
    ImGui::SliderFloat("Bloom Radius", &postProcessSettings.bloomRadius, 0.5f, 8.0f);

    ImGui::Separator();

    ImGui::Text("Debug View: %s", ToString(debugSettings.debugViewMode));

    if (ImGui::Button("None")) {
      debugSettings.debugViewMode = DebugViewMode::None;
    }

    ImGui::SameLine();

    if (ImGui::Button("Normals (F4)")) {
      debugSettings.debugViewMode = DebugViewMode::Normals;
    }

    ImGui::SameLine();

    if (ImGui::Button("Depth (F5)")) {
      debugSettings.debugViewMode = DebugViewMode::Depth;
    }

    ImGui::SliderFloat("Depth Visualization Range", &postProcessSettings.depthVisualizationRange, 1.0f, 100.0f);

    ImGui::Separator();

    ImGui::SliderFloat("Exposure", &postProcessSettings.exposure, 0.1f, 3.0f);
    ImGui::SliderFloat("Contrast", &postProcessSettings.contrast, 0.1f, 3.0f);

    ImGui::Separator();

    ImGui::Text("Scene Objects");

    std::vector<SceneObject>& sceneObjects = scene.GetObjects();

    if (ImGui::CollapsingHeader("Lighting", ImGuiTreeNodeFlags_DefaultOpen)) {
      DirectionalLight& light = scene.GetDirectionalLight();

      ImGui::DragFloat3("Light Direction", &light.direction.x, 0.05f, -1.0f, 1.0f);

      ImGui::ColorEdit3("Light Color", &light.color.x);

      ImGui::SliderFloat("Light Intensity", &light.intensity, 0.0f, 5.0f);

      ImGui::ColorEdit3("Ambient Color", &light.ambientColor.x);

      ImGui::SliderFloat("Ambient Intensity", &light.ambientIntensity, 0.0f, 1.0f);
    }

    for (SceneObject& object : sceneObjects) {
      ImGui::PushID(object.name.c_str());

      if (ImGui::TreeNode(object.name.c_str())) {

        const MaterialHandle materialHandle = object.materialInstance.material;
        const MaterialShaderType shaderType = renderResources.GetMaterialShaderType(materialHandle);

        ImGui::Text("Current Material: %s", renderResources.GetMaterialDebugName(materialHandle));

        ImGui::Text("Shader Type: %s", ToString(shaderType));

        const TextureHandle textureHandle = renderResources.GetMaterialBaseTexture(materialHandle);

        ImGui::Text("Texture: %s", renderResources.GetTextureDebugName(textureHandle));

        ImGui::Text("Texture Size: %u x %u",
                    renderResources.GetTextureWidth(textureHandle),
                    renderResources.GetTextureHeight(textureHandle));

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

        const MeshHandle meshHandle = object.mesh;

        ImGui::Text("Mesh: %s", renderResources.GetMeshDebugName(meshHandle));

        ImGui::Text("Mesh Vertices: %u", renderResources.GetMeshVertexCount(meshHandle));

        ImGui::Text("Mesh Indices: %u", renderResources.GetMeshIndexCount(meshHandle));

        const uint32_t meshCount = renderResources.GetMeshCount();

        const char* currentMeshName = renderResources.GetMeshDebugName(meshHandle);

        if (ImGui::BeginCombo("Mesh", currentMeshName)) {
          for (uint32_t meshIndex = 0; meshIndex < meshCount; ++meshIndex) {
            MeshHandle candidateHandle = renderResources.GetMeshHandleAt(meshIndex);

            const bool isSelected = candidateHandle.value == object.mesh.value;

            const char* candidateName = renderResources.GetMeshDebugName(candidateHandle);

            if (ImGui::Selectable(candidateName, isSelected)) {
              object.mesh = candidateHandle;
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

        ImGui::SliderFloat("Specular Strength", &object.materialInstance.specularStrength, 0.0f, 2.0f);

        ImGui::SliderFloat("Specular Power", &object.materialInstance.specularPower, 1.0f, 128.0f);

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