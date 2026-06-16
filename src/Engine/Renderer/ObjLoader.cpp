#include "Engine/Renderer/ObjLoader.h"

#include "Engine/Core/Log.h"
#include "Engine/Renderer/MeshData.h"

#include <DirectXMath.h>

#include <fstream>
#include <sstream>
#include <string>
#include <unordered_map>
#include <vector>

namespace Engine::ObjLoader
{
    namespace
    {
        std::vector<std::string> Split(const std::string& text, char delimiter)
        {
            std::vector<std::string> result;
            std::stringstream stream(text);
            std::string item;

            while (std::getline(stream, item, delimiter))
            {
                result.push_back(item);
            }

            return result;
        }

        int ParseObjIndex(const std::string& value)
        {
            if (value.empty())
            {
                return -1;
            }

            const int parsed = std::stoi(value);

            // For now we support normal positive OBJ indices only.
            if (parsed <= 0)
            {
                return -1;
            }

            return parsed - 1;
        }

        bool AddVertexFromFaceToken(
            const std::string& token,
            const std::vector<DirectX::XMFLOAT3>& positions,
            const std::vector<DirectX::XMFLOAT2>& texcoords,
            const std::vector<DirectX::XMFLOAT3>& normals,
            std::unordered_map<std::string, uint32_t>& vertexCache,
            MeshData& meshData)
        {
            const auto cached = vertexCache.find(token);

            if (cached != vertexCache.end())
            {
                meshData.indices.push_back(cached->second);
                return true;
            }

            const std::vector<std::string> parts = Split(token, '/');

            if (parts.empty())
            {
                return false;
            }

            const int positionIndex = ParseObjIndex(parts[0]);
            const int texcoordIndex = parts.size() > 1 ? ParseObjIndex(parts[1]) : -1;
            const int normalIndex = parts.size() > 2 ? ParseObjIndex(parts[2]) : -1;

            if (positionIndex < 0 || positionIndex >= static_cast<int>(positions.size()))
            {
                return false;
            }

            VertexPositionColor vertex = {};
            vertex.position[0] = positions[positionIndex].x;
            vertex.position[1] = positions[positionIndex].y;
            vertex.position[2] = positions[positionIndex].z;

            vertex.color[0] = 1.0f;
            vertex.color[1] = 1.0f;
            vertex.color[2] = 1.0f;

            if (normalIndex >= 0 && normalIndex < static_cast<int>(normals.size()))
            {
                vertex.normal[0] = normals[normalIndex].x;
                vertex.normal[1] = normals[normalIndex].y;
                vertex.normal[2] = normals[normalIndex].z;
            }
            else
            {
                vertex.normal[0] = 0.0f;
                vertex.normal[1] = 1.0f;
                vertex.normal[2] = 0.0f;
            }

            if (texcoordIndex >= 0 && texcoordIndex < static_cast<int>(texcoords.size()))
            {
                vertex.texcoord[0] = texcoords[texcoordIndex].x;
                vertex.texcoord[1] = 1.0f - texcoords[texcoordIndex].y;
            }
            else
            {
                vertex.texcoord[0] = 0.0f;
                vertex.texcoord[1] = 0.0f;
            }

            const uint32_t newIndex = static_cast<uint32_t>(meshData.vertices.size());
            meshData.vertices.push_back(vertex);
            meshData.indices.push_back(newIndex);

            vertexCache[token] = newIndex;
            return true;
        }
    }

    bool Load(const char* filePath, MeshData& meshData)
    {
        meshData.vertices.clear();
        meshData.indices.clear();

        std::ifstream file(filePath);

        if (!file.is_open())
        {
            LogWarning("Failed to open OBJ file.");
            return false;
        }

        std::vector<DirectX::XMFLOAT3> positions;
        std::vector<DirectX::XMFLOAT2> texcoords;
        std::vector<DirectX::XMFLOAT3> normals;
        std::unordered_map<std::string, uint32_t> vertexCache;

        std::string line;

        while (std::getline(file, line))
        {
            if (line.empty() || line[0] == '#')
            {
                continue;
            }

            std::istringstream lineStream(line);
            std::string type;
            lineStream >> type;

            if (type == "v")
            {
                DirectX::XMFLOAT3 position = {};
                lineStream >> position.x >> position.y >> position.z;
                positions.push_back(position);
            }
            else if (type == "vt")
            {
                DirectX::XMFLOAT2 texcoord = {};
                lineStream >> texcoord.x >> texcoord.y;
                texcoords.push_back(texcoord);
            }
            else if (type == "vn")
            {
                DirectX::XMFLOAT3 normal = {};
                lineStream >> normal.x >> normal.y >> normal.z;
                normals.push_back(normal);
            }
            else if (type == "f")
            {
                std::vector<std::string> faceTokens;
                std::string token;

                while (lineStream >> token)
                {
                    faceTokens.push_back(token);
                }

                if (faceTokens.size() < 3)
                {
                    continue;
                }

                for (size_t i = 1; i + 1 < faceTokens.size(); ++i)
                {
                    if (!AddVertexFromFaceToken(faceTokens[0], positions, texcoords, normals, vertexCache, meshData) ||
                        !AddVertexFromFaceToken(faceTokens[i], positions, texcoords, normals, vertexCache, meshData) ||
                        !AddVertexFromFaceToken(faceTokens[i + 1], positions, texcoords, normals, vertexCache, meshData))
                    {
                        LogWarning("Failed to parse OBJ face.");
                        return false;
                    }
                }
            }
        }

        if (!meshData.IsValid())
        {
            LogWarning("OBJ file produced no valid mesh data.");
            return false;
        }

        LogInfo("OBJ mesh loaded.");
        return true;
    }
}