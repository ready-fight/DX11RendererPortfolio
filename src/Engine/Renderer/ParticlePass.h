#pragma once

#include "Engine/Renderer/GpuBuffer.h"
#include "Engine/Renderer/ShaderProgram.h"

#include <DirectXMath.h>

namespace Engine {
  class Camera;
  class GraphicsDevice;
  struct ParticleSystemSettings;

  class ParticlePass final {
  public:
    bool Initialize(GraphicsDevice& graphicsDevice);
    void Shutdown();

    void Render(GraphicsDevice& graphicsDevice, const Camera& camera, const ParticleSystemSettings& settings,
                float totalTime);

  private:
    ShaderProgram m_shader;
    GpuBuffer m_particleBuffer;
  };
}