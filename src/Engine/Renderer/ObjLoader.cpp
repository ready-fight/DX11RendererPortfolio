#include "Engine/Renderer/ObjLoader.h"

#include "Engine/Core/Log.h"
#include "Engine/Renderer/MeshData.h"

#include <DirectXMath.h>

#include <fstream>
#include <sstream>
#include <string>
#include <unordered_map>
#include <vector>

namespace Engine::ObjLoader {
  namespace {

    struct FaceVertex {
      int positionIndex = -1;
      int texcoordIndex = -1;
      int normalIndex = -1;
      std::string cacheKey;
    };

    std::vector<std::string> Split(const std::string& text, char delimiter) {
      std::vector<std::string> result;
      std::stringstream stream(text);
      std::string item;

      while (std::getline(stream, item, delimiter)) {
        result.push_back(item);
      }

      return result;
    }

    int ParseObjIndex(const std::string& value) {
      if (value.empty()) {
        return -1;
      }

      const int parsed = std::stoi(value);

      // For now we support normal positive OBJ indices only.
      if (parsed <= 0) {
        return -1;
      }

      return parsed - 1;
    }

    bool ParseFaceVertex(const std::string& token, FaceVertex& faceVertex) {
      const std::vector<std::string> parts = Split(token, '/');

      if (parts.empty()) {
        return false;
      }

      faceVertex.positionIndex = ParseObjIndex(parts[0]);
      faceVertex.texcoordIndex = parts.size() > 1 ? ParseObjIndex(parts[1]) : -1;
      faceVertex.normalIndex = parts.size() > 2 ? ParseObjIndex(parts[2]) : -1;
      faceVertex.cacheKey = token;

      return faceVertex.positionIndex >= 0;
    }

    DirectX::XMFLOAT3 CalculateFaceNormal(const DirectX::XMFLOAT3& a, const DirectX::XMFLOAT3& b,
                                          const DirectX::XMFLOAT3& c) {
      using namespace DirectX;

      const XMVECTOR va = XMLoadFloat3(&a);
      const XMVECTOR vb = XMLoadFloat3(&b);
      const XMVECTOR vc = XMLoadFloat3(&c);

      const XMVECTOR edge0 = vb - va;
      const XMVECTOR edge1 = vc - va;

      XMVECTOR normal = XMVector3Cross(edge0, edge1);
      normal = XMVector3Normalize(normal);

      XMFLOAT3 result = {};
      XMStoreFloat3(&result, normal);
      return result;
    }

    bool AddVertex(const FaceVertex& faceVertex, const DirectX::XMFLOAT3& fallbackNormal,
                   const std::vector<DirectX::XMFLOAT3>& positions, const std::vector<DirectX::XMFLOAT2>& texcoords,
                   const std::vector<DirectX::XMFLOAT3>& normals,
                   std::unordered_map<std::string, uint32_t>& vertexCache, MeshData& meshData) {
      std::string cacheKey = faceVertex.cacheKey;

      if (faceVertex.normalIndex < 0) {
        cacheKey += "_generated_normal";
        cacheKey += std::to_string(meshData.indices.size());
      }

      const auto cached = vertexCache.find(cacheKey);

      if (cached != vertexCache.end()) {
        meshData.indices.push_back(cached->second);
        return true;
      }

      if (faceVertex.positionIndex < 0 || faceVertex.positionIndex >= static_cast<int>(positions.size())) {
        return false;
      }

      VertexPositionColor vertex = {};

      const DirectX::XMFLOAT3& position = positions[faceVertex.positionIndex];

      vertex.position[0] = position.x;
      vertex.position[1] = position.y;
      vertex.position[2] = position.z;

      vertex.color[0] = 1.0f;
      vertex.color[1] = 1.0f;
      vertex.color[2] = 1.0f;

      if (faceVertex.normalIndex >= 0 && faceVertex.normalIndex < static_cast<int>(normals.size())) {
        const DirectX::XMFLOAT3& normal = normals[faceVertex.normalIndex];

        vertex.normal[0] = normal.x;
        vertex.normal[1] = normal.y;
        vertex.normal[2] = normal.z;
      } else {
        vertex.normal[0] = fallbackNormal.x;
        vertex.normal[1] = fallbackNormal.y;
        vertex.normal[2] = fallbackNormal.z;
      }

      if (faceVertex.texcoordIndex >= 0 && faceVertex.texcoordIndex < static_cast<int>(texcoords.size())) {
        const DirectX::XMFLOAT2& texcoord = texcoords[faceVertex.texcoordIndex];

        vertex.texcoord[0] = texcoord.x;
        vertex.texcoord[1] = 1.0f - texcoord.y;
      } else {
        vertex.texcoord[0] = 0.0f;
        vertex.texcoord[1] = 0.0f;
      }

      const uint32_t newIndex = static_cast<uint32_t>(meshData.vertices.size());

      meshData.vertices.push_back(vertex);
      meshData.indices.push_back(newIndex);

      vertexCache[cacheKey] = newIndex;
      return true;
    }
  }

  bool Load(const char* filePath, MeshData& meshData) {
    meshData.vertices.clear();
    meshData.indices.clear();

    std::ifstream file(filePath);

    if (!file.is_open()) {
      LogWarning("Failed to open OBJ file.");
      return false;
    }

    std::vector<DirectX::XMFLOAT3> positions;
    std::vector<DirectX::XMFLOAT2> texcoords;
    std::vector<DirectX::XMFLOAT3> normals;
    std::unordered_map<std::string, uint32_t> vertexCache;

    std::string line;
    uint32_t lineNumber = 0;

    while (std::getline(file, line)) {
      ++lineNumber;

      if (line.empty() || line[0] == '#') {
        continue;
      }

      std::istringstream lineStream(line);
      std::string type;
      lineStream >> type;

      if (type == "v") {
        DirectX::XMFLOAT3 position = {};
        lineStream >> position.x >> position.y >> position.z;
        positions.push_back(position);
      } else if (type == "vt") {
        DirectX::XMFLOAT2 texcoord = {};
        lineStream >> texcoord.x >> texcoord.y;
        texcoords.push_back(texcoord);
      } else if (type == "vn") {
        DirectX::XMFLOAT3 normal = {};
        lineStream >> normal.x >> normal.y >> normal.z;
        normals.push_back(normal);
      } else if (type == "f") {
        std::vector<FaceVertex> faceVertices;
        std::string token;

        while (lineStream >> token) {
          FaceVertex faceVertex = {};

          if (!ParseFaceVertex(token, faceVertex)) {
            LogWarning("Failed to parse OBJ face vertex.");
            return false;
          }

          faceVertices.push_back(faceVertex);
        }

        if (faceVertices.size() < 3) {
          continue;
        }

        for (size_t i = 1; i + 1 < faceVertices.size(); ++i) {
          const FaceVertex triangleVertices[3] = {faceVertices[0], faceVertices[i], faceVertices[i + 1]};

          const DirectX::XMFLOAT3& p0 = positions[triangleVertices[0].positionIndex];
          const DirectX::XMFLOAT3& p1 = positions[triangleVertices[1].positionIndex];
          const DirectX::XMFLOAT3& p2 = positions[triangleVertices[2].positionIndex];

          const DirectX::XMFLOAT3 fallbackNormal = CalculateFaceNormal(p0, p1, p2);

          if (!AddVertex(triangleVertices[0], fallbackNormal, positions, texcoords, normals, vertexCache, meshData) ||
              !AddVertex(triangleVertices[1], fallbackNormal, positions, texcoords, normals, vertexCache, meshData) ||
              !AddVertex(triangleVertices[2], fallbackNormal, positions, texcoords, normals, vertexCache, meshData)) {
            LogWarning("Failed to build OBJ triangle.");
            return false;
          }
        }
      }
    }

    if (!meshData.IsValid()) {
      LogWarning("OBJ file produced no valid mesh data.");
      return false;
    }

    LogInfo("OBJ mesh loaded.");
    return true;
  }
}