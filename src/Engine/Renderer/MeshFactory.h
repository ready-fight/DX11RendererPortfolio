#pragma once

namespace Engine {
  class GraphicsDevice;
  class Mesh;
  struct MeshData;

  namespace MeshFactory {
    bool CreateCubeMeshData(MeshData& meshData);
    bool CreateCube(GraphicsDevice& graphicsDevice, Mesh& mesh);
  }
}