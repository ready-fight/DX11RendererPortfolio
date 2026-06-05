#include "Engine/Renderer/GpuBuffer.h"
#include "Engine/Renderer/ShaderProgram.h"

namespace Engine
{
    class GraphicsDevice;

    class TrianglePass final
    {
    public:
        bool Initialize(GraphicsDevice& graphicsDevice);
        void Shutdown();

        void Render(GraphicsDevice& graphicsDevice);

    private:
        ShaderProgram m_shader;
        GpuBuffer m_vertexBuffer;
    };
}