#include "Engine/Renderer/MeshFactory.h"

#include "Engine/Renderer/Mesh.h"
#include "Engine/Renderer/VertexTypes.h"

#include <cstdint>
#include <iterator>

namespace Engine::MeshFactory {
  bool CreateCube(GraphicsDevice& graphicsDevice, Mesh& mesh) {
    const VertexPositionColor vertices[] = {// Front face
                                            {{-0.5f, -0.5f, -0.5f}, {1.0f, 0.2f, 0.2f}, {0.0f, 0.0f, -1.0f}},
                                            {{-0.5f, 0.5f, -0.5f}, {1.0f, 0.2f, 0.2f}, {0.0f, 0.0f, -1.0f}},
                                            {{0.5f, 0.5f, -0.5f}, {1.0f, 0.2f, 0.2f}, {0.0f, 0.0f, -1.0f}},
                                            {{0.5f, -0.5f, -0.5f}, {1.0f, 0.2f, 0.2f}, {0.0f, 0.0f, -1.0f}},

                                            // Back face
                                            {{-0.5f, -0.5f, 0.5f}, {0.2f, 1.0f, 0.2f}, {0.0f, 0.0f, 1.0f}},
                                            {{0.5f, -0.5f, 0.5f}, {0.2f, 1.0f, 0.2f}, {0.0f, 0.0f, 1.0f}},
                                            {{0.5f, 0.5f, 0.5f}, {0.2f, 1.0f, 0.2f}, {0.0f, 0.0f, 1.0f}},
                                            {{-0.5f, 0.5f, 0.5f}, {0.2f, 1.0f, 0.2f}, {0.0f, 0.0f, 1.0f}},

                                            // Left face
                                            {{-0.5f, -0.5f, 0.5f}, {0.2f, 0.4f, 1.0f}, {-1.0f, 0.0f, 0.0f}},
                                            {{-0.5f, 0.5f, 0.5f}, {0.2f, 0.4f, 1.0f}, {-1.0f, 0.0f, 0.0f}},
                                            {{-0.5f, 0.5f, -0.5f}, {0.2f, 0.4f, 1.0f}, {-1.0f, 0.0f, 0.0f}},
                                            {{-0.5f, -0.5f, -0.5f}, {0.2f, 0.4f, 1.0f}, {-1.0f, 0.0f, 0.0f}},

                                            // Right face
                                            {{0.5f, -0.5f, -0.5f}, {1.0f, 1.0f, 0.2f}, {1.0f, 0.0f, 0.0f}},
                                            {{0.5f, 0.5f, -0.5f}, {1.0f, 1.0f, 0.2f}, {1.0f, 0.0f, 0.0f}},
                                            {{0.5f, 0.5f, 0.5f}, {1.0f, 1.0f, 0.2f}, {1.0f, 0.0f, 0.0f}},
                                            {{0.5f, -0.5f, 0.5f}, {1.0f, 1.0f, 0.2f}, {1.0f, 0.0f, 0.0f}},

                                            // Top face
                                            {{-0.5f, 0.5f, -0.5f}, {1.0f, 0.2f, 1.0f}, {0.0f, 1.0f, 0.0f}},
                                            {{-0.5f, 0.5f, 0.5f}, {1.0f, 0.2f, 1.0f}, {0.0f, 1.0f, 0.0f}},
                                            {{0.5f, 0.5f, 0.5f}, {1.0f, 0.2f, 1.0f}, {0.0f, 1.0f, 0.0f}},
                                            {{0.5f, 0.5f, -0.5f}, {1.0f, 0.2f, 1.0f}, {0.0f, 1.0f, 0.0f}},

                                            // Bottom face
                                            {{-0.5f, -0.5f, 0.5f}, {0.2f, 1.0f, 1.0f}, {0.0f, -1.0f, 0.0f}},
                                            {{-0.5f, -0.5f, -0.5f}, {0.2f, 1.0f, 1.0f}, {0.0f, -1.0f, 0.0f}},
                                            {{0.5f, -0.5f, -0.5f}, {0.2f, 1.0f, 1.0f}, {0.0f, -1.0f, 0.0f}},
                                            {{0.5f, -0.5f, 0.5f}, {0.2f, 1.0f, 1.0f}, {0.0f, -1.0f, 0.0f}}};

    const uint32_t indices[] = {0,  1,  2,  0,  2,  3,  4,  5,  6,  4,  6,  7,  8,  9,  10, 8,  10, 11,
                                12, 13, 14, 12, 14, 15, 16, 17, 18, 16, 18, 19, 20, 21, 22, 20, 22, 23};

    return mesh.Initialize(graphicsDevice,
                           vertices,
                           static_cast<uint32_t>(std::size(vertices)),
                           indices,
                           static_cast<uint32_t>(std::size(indices)));
  }
}