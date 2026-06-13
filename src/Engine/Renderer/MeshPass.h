#pragma once

#include "Engine/Renderer/Camera.h"
#include "Engine/Renderer/GpuBuffer.h"
#include "Engine/Renderer/Texture2D.h"

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
        Texture2D m_checkerTexture;

        GpuBuffer m_transformBuffer;
        GpuBuffer m_materialBuffer;
        GpuBuffer m_lightBuffer;
    };
}