#include "Engine/Renderer/Material.h"

#include "Engine/Core/Log.h"
#include "Engine/Renderer/GraphicsDevice.h"

namespace Engine
{
    bool Material::Initialize(
        GraphicsDevice& graphicsDevice,
        const wchar_t* shaderPath,
        const D3D11_INPUT_ELEMENT_DESC* inputElements,
        unsigned int inputElementCount)
    {
        if (!m_shader.Initialize(
            graphicsDevice,
            shaderPath,
            inputElements,
            inputElementCount))
        {
            return false;
        }

        LogInfo("Material initialized.");
        return true;
    }

    void Material::Shutdown()
    {
        m_shader.Shutdown();
    }

    void Material::Bind(GraphicsDevice& graphicsDevice)
    {
        m_shader.Bind(graphicsDevice);
    }
}