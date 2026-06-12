#pragma once

#include "Engine/Scene/SceneObject.h"

#include <vector>

namespace Engine
{
    class Scene final
    {
    public:
        void Clear();

        SceneObject& AddObject(const SceneObject& object);

        std::vector<SceneObject>& GetObjects() { return m_objects; }
        const std::vector<SceneObject>& GetObjects() const { return m_objects; }

    private:
        std::vector<SceneObject> m_objects;
    };
}