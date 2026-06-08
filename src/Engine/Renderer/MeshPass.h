#include "Engine/Renderer/Camera.h"
#include "Engine/Renderer/GpuBuffer.h"
#include "Engine/Renderer/ShaderProgram.h"

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
        GpuBuffer m_vertexBuffer;
        GpuBuffer m_indexBuffer;
        GpuBuffer m_transformBuffer;
    };
}