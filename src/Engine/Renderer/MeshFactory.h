#pragma once

namespace Engine
{
    class GraphicsDevice;
    class Mesh;

    namespace MeshFactory
    {
        bool CreateCube(GraphicsDevice& graphicsDevice, Mesh& mesh);
    }
}