#pragma once

#include "Engine/Renderer/ShaderProgram.h"

#include <d3d11.h>

namespace Engine
{
    class GraphicsDevice;

    class Material final
    {
    public:
        bool Initialize(
            GraphicsDevice& graphicsDevice,
            const wchar_t* shaderPath,
            const D3D11_INPUT_ELEMENT_DESC* inputElements,
            unsigned int inputElementCount
        );

        void Shutdown();

        void Bind(GraphicsDevice& graphicsDevice);

    private:
        ShaderProgram m_shader;
    };
}