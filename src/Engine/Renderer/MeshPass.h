#pragma once

#include "Engine/Renderer/Camera.h"
#include "Engine/Renderer/GpuBuffer.h"

namespace Engine
{
    class GraphicsDevice;
    class RenderResourceManager;
    class Scene;

    class MeshPass final
    {
    public:
        bool Initialize(GraphicsDevice& graphicsDevice);
        void Shutdown();

        void Render(
            GraphicsDevice& graphicsDevice,
            RenderResourceManager& renderResources,
            const Camera& camera,
            Scene& scene,
            float totalSeconds
        );

    private:
        GpuBuffer m_transformBuffer;
        GpuBuffer m_lightBuffer;
    };
}