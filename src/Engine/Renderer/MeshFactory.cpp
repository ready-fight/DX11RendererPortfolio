#include "Engine/Renderer/MeshFactory.h"

#include "Engine/Renderer/Mesh.h"
#include "Engine/Renderer/VertexTypes.h"

#include <cstdint>
#include <iterator>

namespace Engine::MeshFactory
{
    bool CreateCube(GraphicsDevice& graphicsDevice, Mesh& mesh)
    {
        const VertexPositionColor vertices[] =
        {
            { { -0.5f, -0.5f, -0.5f }, { 1.0f, 0.0f, 0.0f } },
            { { -0.5f,  0.5f, -0.5f }, { 0.0f, 1.0f, 0.0f } },
            { {  0.5f,  0.5f, -0.5f }, { 0.0f, 0.0f, 1.0f } },
            { {  0.5f, -0.5f, -0.5f }, { 1.0f, 1.0f, 0.0f } },

            { { -0.5f, -0.5f,  0.5f }, { 1.0f, 0.0f, 1.0f } },
            { { -0.5f,  0.5f,  0.5f }, { 0.0f, 1.0f, 1.0f } },
            { {  0.5f,  0.5f,  0.5f }, { 1.0f, 1.0f, 1.0f } },
            { {  0.5f, -0.5f,  0.5f }, { 0.3f, 0.3f, 0.3f } }
        };

        const uint32_t indices[] =
        {
            0, 1, 2, 0, 2, 3,
            4, 6, 5, 4, 7, 6,

            4, 5, 1, 4, 1, 0,
            3, 2, 6, 3, 6, 7,

            1, 5, 6, 1, 6, 2,
            4, 0, 3, 4, 3, 7
        };

        return mesh.Initialize(
            graphicsDevice,
            vertices,
            static_cast<uint32_t>(std::size(vertices)),
            indices,
            static_cast<uint32_t>(std::size(indices))
        );
    }
}