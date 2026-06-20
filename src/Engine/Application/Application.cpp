#include "Engine/Application/Application.h"
#include "Engine/Core/Log.h"
#include "Engine/Renderer/PostProcessSettings.h"

#include <Windows.h>

#include <objbase.h>

namespace Engine {
  bool Application::Initialize() {

    HRESULT comResult = CoInitializeEx(nullptr, COINIT_MULTITHREADED);

    if (FAILED(comResult)) {
      MessageBoxW(nullptr, L"Failed to initialize COM.", L"Error", MB_OK);
      return false;
    }

    WindowDesc windowDesc = {};
    windowDesc.title = L"DX11 Renderer Portfolio";
    windowDesc.width = 1280;
    windowDesc.height = 720;

    if (!m_window.Create(windowDesc)) {
      MessageBoxW(nullptr, L"Failed to create window.", L"Error", MB_OK);
      return false;
    }

    const float aspectRatio = static_cast<float>(m_window.GetWidth()) / static_cast<float>(m_window.GetHeight());

    m_camera.SetTarget(0.0f, 0.0f, 0.0f);
    m_camera.SetOrbit(0.0f, 0.15f, 4.5f);
    m_camera.SetPerspective(DirectX::XMConvertToRadians(60.0f), aspectRatio, 0.1f, 100.0f);

    if (!m_graphicsDevice.Initialize(m_window.GetNativeHandle(), m_window.GetWidth(), m_window.GetHeight())) {
      MessageBoxW(nullptr, L"Failed to initialize DirectX 11.", L"Error", MB_OK);
      return false;
    }

    if (!m_sceneRenderTarget.InitializeColor(
            m_graphicsDevice, m_window.GetWidth(), m_window.GetHeight(), DXGI_FORMAT_R16G16B16A16_FLOAT)) {
      MessageBoxW(nullptr, L"Failed to initialize scene render target.", L"Error", MB_OK);
      return false;
    }

    if (!m_fullscreenPass.Initialize(m_graphicsDevice)) {
      MessageBoxW(nullptr, L"Failed to initialize fullscreen pass.", L"Error", MB_OK);
      return false;
    }

    if (!m_debugOverlay.Initialize(m_window.GetNativeHandle(), m_graphicsDevice)) {
      MessageBoxW(nullptr, L"Failed to initialize debug overlay.", L"Error", MB_OK);
      return false;
    }

    if (!m_renderStates.Initialize(m_graphicsDevice)) {
      return false;
    }

    if (!m_renderResources.Initialize(m_graphicsDevice)) {
      MessageBoxW(nullptr, L"Failed to initialize render resources.", L"Error", MB_OK);
      return false;
    }

    if (!m_meshPass.Initialize(m_graphicsDevice)) {
      MessageBoxW(nullptr, L"Failed to initialize mesh pass.", L"Error", MB_OK);
      return false;
    }

    if (!m_particlePass.Initialize(m_graphicsDevice)) {
      return false;
    }

    BuildTestScene();

    m_isRunning = true;

    m_postProcessSettings.grayscaleAmount = 0.0f;
    m_postProcessSettings.exposure = 1.0f;
    m_postProcessSettings.contrast = 1.0f;
    m_postProcessSettings.vignetteAmount = 0.0f;

    return true;
  }

  void Application::Shutdown() {

    m_scene.Clear();

    m_meshPass.Shutdown();
    m_renderResources.Shutdown();

    m_debugOverlay.Shutdown();
    m_fullscreenPass.Shutdown();
    m_particlePass.Shutdown();
    m_sceneRenderTarget.Shutdown();
    m_renderStates.Shutdown();
    m_graphicsDevice.Shutdown();
    m_window.Destroy();
    m_isRunning = false;

    CoUninitialize();
  }

  int Application::Run() {
    while (m_isRunning) {
      if (!m_window.ProcessMessages()) {
        m_isRunning = false;
        break;
      }

      m_timer.Tick();

      Tick(m_timer.GetDeltaSeconds());
      Render();
    }

    Shutdown();
    return 0;
  }

  void Application::Tick(float deltaSeconds) {

    m_keyboardInput.Update();

    if (m_keyboardInput.WasPressed(VK_F1)) {
      m_debugSettings.wireframeEnabled = !m_debugSettings.wireframeEnabled;

      LogInfo(m_debugSettings.wireframeEnabled ? "Wireframe mode enabled." : "Wireframe mode disabled.");
    }

    if (m_keyboardInput.WasPressed(VK_F2)) {
      m_debugSettings.grayscaleEnabled = !m_debugSettings.grayscaleEnabled;

      LogInfo(m_debugSettings.grayscaleEnabled ? "Grayscale post effect enabled." : "Grayscale post effect disabled.");
    }

    if (m_keyboardInput.WasPressed(VK_F3)) {
      m_debugSettings.vignetteEnabled = !m_debugSettings.vignetteEnabled;

      LogInfo(m_debugSettings.vignetteEnabled ? "Vignette post effect enabled." : "Vignette post effect disabled.");
    }

    if (m_keyboardInput.WasPressed(VK_F4)) {
      m_debugSettings.debugViewMode =
          m_debugSettings.debugViewMode == DebugViewMode::Normals ? DebugViewMode::None : DebugViewMode::Normals;

      // TODO: Add LogInfo
    }

    if (m_keyboardInput.WasPressed(VK_F5)) {
      m_debugSettings.debugViewMode =
          m_debugSettings.debugViewMode == DebugViewMode::Depth ? DebugViewMode::None : DebugViewMode::Depth;

      // TODO: Add LogInfo
    }

    if (m_keyboardInput.WasPressed(VK_F6)) {
      m_debugSettings.bloomEnabled = !m_debugSettings.bloomEnabled;

      // TODO: Add LogInfo
    }

    if (m_keyboardInput.WasPressed(VK_F7)) {
      m_debugSettings.particlesEnabled = !m_debugSettings.particlesEnabled;

      // TODO: Add LogInfo
    }

    if (m_keyboardInput.WasPressed(VK_F8)) {
      m_debugSettings.portfolioInfoEnabled = !m_debugSettings.portfolioInfoEnabled;
    }

    const float orbitSpeed = 1.5f * deltaSeconds;
    const float zoomSpeed = 3.0f * deltaSeconds;

    if (m_keyboardInput.IsDown(VK_LEFT)) {
      m_camera.AddOrbit(-orbitSpeed, 0.0f);
    }

    if (m_keyboardInput.IsDown(VK_RIGHT)) {
      m_camera.AddOrbit(orbitSpeed, 0.0f);
    }

    if (m_keyboardInput.IsDown(VK_UP)) {
      m_camera.AddOrbit(0.0f, orbitSpeed);
    }

    if (m_keyboardInput.IsDown(VK_DOWN)) {
      m_camera.AddOrbit(0.0f, -orbitSpeed);
    }

    if (m_keyboardInput.IsDown('Q')) {
      m_camera.AddDistance(-zoomSpeed);
    }

    if (m_keyboardInput.IsDown('E')) {
      m_camera.AddDistance(zoomSpeed);
    }

    m_postProcessSettings.grayscaleAmount = m_debugSettings.grayscaleEnabled ? 1.0f : 0.0f;

    m_postProcessSettings.vignetteAmount = m_debugSettings.vignetteEnabled ? 1.0f : 0.0f;

    m_postProcessSettings.depthVisualizationAmount =
        m_debugSettings.debugViewMode == DebugViewMode::Depth ? 1.0f : 0.0f;
  }

  void Application::Render() {

    m_graphicsDevice.BeginFrameStats(m_timer.GetDeltaSeconds());

    m_graphicsDevice.BeginRenderPass(RenderPassType::Scene);

    m_graphicsDevice.SetRenderTarget(m_sceneRenderTarget, &m_graphicsDevice.GetDepthStencilBuffer());

    m_graphicsDevice.ClearRenderTarget(m_sceneRenderTarget, 0.05f, 0.08f, 0.12f, 1.0f);

    m_graphicsDevice.ClearDepthStencil(m_graphicsDevice.GetDepthStencilBuffer());

    m_renderStates.Apply(m_graphicsDevice, m_debugSettings.wireframeEnabled, BlendMode::Opaque);

    m_meshPass.Render(m_graphicsDevice,
                      m_renderResources,
                      m_camera,
                      m_scene,
                      m_timer.GetTotalSeconds(),
                      m_debugSettings.debugViewMode == DebugViewMode::Normals);

    if (m_debugSettings.particlesEnabled && m_debugSettings.debugViewMode != DebugViewMode::Normals) {
      m_renderStates.Apply(m_graphicsDevice, false, BlendMode::Additive);

      m_particlePass.Render(m_graphicsDevice, m_camera, m_scene.GetParticleSystemSettings(), m_timer.GetTotalSeconds());
    }

    m_graphicsDevice.BeginRenderPass(RenderPassType::PostProcess);

    m_graphicsDevice.SetBackBufferRenderTarget();

    m_graphicsDevice.ClearRenderTarget(m_graphicsDevice.GetBackBufferRenderTarget(), 0.0f, 0.0f, 0.0f, 1.0f);

    m_renderStates.Apply(m_graphicsDevice, false, BlendMode::Opaque);

    m_fullscreenPass.Render(m_graphicsDevice,
                            m_sceneRenderTarget,
                            m_graphicsDevice.GetDepthStencilBuffer(),
                            m_postProcessSettings,
                            m_debugSettings);

    m_debugOverlay.BeginFrame();

    m_debugOverlay.Draw(m_debugSettings,
                        m_postProcessSettings,
                        m_graphicsDevice.GetRenderStats(),
                        m_renderResources,
                        m_scene,
                        m_graphicsDevice.GetFPS());

    m_debugOverlay.EndFrame();

    m_graphicsDevice.EndFrame();
  }

  void Application::BuildTestScene() {
    m_scene.Clear();

    SceneObject leftCube = {};
    leftCube.name = "Left Cube";
    leftCube.mesh = m_renderResources.GetCubeMeshHandle();
    leftCube.materialInstance.material = m_renderResources.GetRedMaterialHandle();
    leftCube.transform.position = {-1.5f, 0.0f, 0.0f};
    leftCube.transform.scale = {0.75f, 0.75f, 0.75f};
    leftCube.materialInstance.baseColor = {1.0f, 0.35f, 0.35f, 1.0f};
    leftCube.rotationSpeed = 0.75f;
    m_scene.AddObject(leftCube);

    SceneObject centerCube = {};
    centerCube.name = "Center Cube";
    centerCube.mesh = m_renderResources.GetModelMeshHandle();
    centerCube.materialInstance.material = m_renderResources.GetGreenMaterialHandle();
    centerCube.transform.position = {0.0f, 0.0f, 0.0f};
    centerCube.transform.scale = {1.0f, 1.0f, 1.0f};
    centerCube.materialInstance.baseColor = {0.35f, 1.0f, 0.35f, 1.0f};
    centerCube.rotationSpeed = 1.25f;
    m_scene.AddObject(centerCube);

    SceneObject rightCube = {};
    rightCube.name = "Right Cube";
    rightCube.mesh = m_renderResources.GetCubeMeshHandle();
    rightCube.materialInstance.material = m_renderResources.GetBlueMaterialHandle();
    rightCube.transform.position = {1.5f, 0.0f, 0.0f};
    rightCube.transform.scale = {0.5f, 0.5f, 0.5f};
    rightCube.materialInstance.baseColor = {0.35f, 0.55f, 1.0f, 1.0f};
    rightCube.rotationSpeed = 2.0f;
    m_scene.AddObject(rightCube);
  }
}