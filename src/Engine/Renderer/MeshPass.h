#pragma once

#include "Engine/Renderer/Camera.h"
#include "Engine/Renderer/GpuBuffer.h"
#include "Engine/Renderer/Mesh.h"
#include "Engine/Renderer/Material.h"
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
        Material m_colorMaterial;
        Mesh m_cubeMesh;
        GpuBuffer m_transformBuffer;
        GpuBuffer m_materialBuffer;
        GpuBuffer m_lightBuffer;

        std::vector<SceneObject> m_sceneObjects;
    };
}