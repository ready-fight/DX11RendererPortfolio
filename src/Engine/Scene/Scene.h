#pragma once

#include "Engine/Scene/DirectionalLight.h"
#include "Engine/Scene/ParticleSystemSettings.h"
#include "Engine/Scene/SceneObject.h"

#include <vector>

namespace Engine {
  class Scene final {
  public:
    void Clear();

    SceneObject& AddObject(const SceneObject& object);

    std::vector<SceneObject>& GetObjects() { return m_objects; }
    const std::vector<SceneObject>& GetObjects() const { return m_objects; }

    DirectionalLight& GetDirectionalLight() { return m_directionalLight; }
    const DirectionalLight& GetDirectionalLight() const { return m_directionalLight; }

    ParticleSystemSettings& GetParticleSystemSettings() { return m_particleSystemSettings; }
    const ParticleSystemSettings& GetParticleSystemSettings() const { return m_particleSystemSettings; }

  private:
    std::vector<SceneObject> m_objects;
    DirectionalLight m_directionalLight;
    ParticleSystemSettings m_particleSystemSettings;
  };
}