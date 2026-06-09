#pragma once

#include "Engine/Renderer/Camera.h"
#include "Engine/Renderer/GpuBuffer.h"
#include "Engine/Renderer/Mesh.h"
#include "Engine/Renderer/ShaderProgram.h"
#include "Engine/Scene/SceneObject.h"

#include <vector>

namespace Engine
{
    class GraphicsDevice;

    class MeshPass final
    {
    public:
        bool Initialize(GraphicsDevice& graphicsDevice);
        void Shutdown();

        void Render(GraphicsDevice& graphicsDevice, const Camera& camera, float totalSeconds);

    private:
        ShaderProgram m_shader;
        Mesh m_cubeMesh;
        GpuBuffer m_transformBuffer;

        std::vector<SceneObject> m_sceneObjects;
    };
}