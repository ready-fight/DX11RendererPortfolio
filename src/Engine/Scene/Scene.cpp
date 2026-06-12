#include "Engine/Scene/Scene.h"

namespace Engine
{
    void Scene::Clear()
    {
        m_objects.clear();
    }

    SceneObject& Scene::AddObject(const SceneObject& object)
    {
        m_objects.push_back(object);
        return m_objects.back();
    }
}