#pragma once

#include "Engine/Renderer/VertexTypes.h"

#include <cstdint>
#include <vector>

namespace Engine
{
    struct MeshData final
    {
        std::vector<VertexPositionColor> vertices;
        std::vector<uint32_t> indices;

        bool IsValid() const
        {
            return !vertices.empty() && !indices.empty();
        }   
    };
}