#pragma once

namespace Engine
{
    struct MeshData;

    namespace ObjLoader
    {
        bool Load(const char* filePath, MeshData& meshData);
    }
}