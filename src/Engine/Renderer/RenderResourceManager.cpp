// src/Engine/Renderer/RenderResourceManager.cpp
#include "Engine/Renderer/RenderResourceManager.h"

#include "Engine/Renderer/MeshFactory.h"
#include "Engine/Renderer/VertexTypes.h"

#include <cstddef>

namespace Engine
{
    bool RenderResourceManager::Initialize(GraphicsDevice& graphicsDevice)
    {
        D3D11_INPUT_ELEMENT_DESC inputElements[] =
        {
            {
                "POSITION",
                0,
                DXGI_FORMAT_R32G32B32_FLOAT,
                0,
                static_cast<UINT>(offsetof(VertexPositionColor, position)),
                D3D11_INPUT_PER_VERTEX_DATA,
                0
            },
            {
                "COLOR",
                0,
                DXGI_FORMAT_R32G32B32_FLOAT,
                0,
                static_cast<UINT>(offsetof(VertexPositionColor, color)),
                D3D11_INPUT_PER_VERTEX_DATA,
                0
            },
            {
                "NORMAL",
                0,
                DXGI_FORMAT_R32G32B32_FLOAT,
                0,
                static_cast<UINT>(offsetof(VertexPositionColor, normal)),
                D3D11_INPUT_PER_VERTEX_DATA,
                0
            },
            {
                "TEXCOORD",
                0,
                DXGI_FORMAT_R32G32_FLOAT,
                0,
                static_cast<UINT>(offsetof(VertexPositionColor, texcoord)),
                D3D11_INPUT_PER_VERTEX_DATA,
                0
            }
        };

        auto colorMaterial = std::make_unique<Material>();

        if (!colorMaterial->Initialize(
            graphicsDevice,
            L"assets/shaders/Color.hlsl",
            inputElements,
            static_cast<unsigned int>(std::size(inputElements))))
        {
            return false;
        }

        m_colorMaterialHandle = AddMaterial(std::move(colorMaterial));

        auto cubeMesh = std::make_unique<Mesh>();

        if (!MeshFactory::CreateCube(graphicsDevice, *cubeMesh))
        {
            return false;
        }

        m_cubeMeshHandle = AddMesh(std::move(cubeMesh));

        return true;
    }

    void RenderResourceManager::Shutdown()
    {
        for (std::unique_ptr<Mesh>& mesh : m_meshes)
        {
            if (mesh)
            {
                mesh->Shutdown();
            }
        }

        for (std::unique_ptr<Material>& material : m_materials)
        {
            if (material)
            {
                material->Shutdown();
            }
        }

        m_meshes.clear();
        m_materials.clear();

        m_cubeMeshHandle = {};
        m_colorMaterialHandle = {};
    }

    MeshHandle RenderResourceManager::AddMesh(std::unique_ptr<Mesh> mesh)
    {
        const uint32_t index = static_cast<uint32_t>(m_meshes.size());
        m_meshes.push_back(std::move(mesh));
        return MeshHandle{ index };
    }

    MaterialHandle RenderResourceManager::AddMaterial(std::unique_ptr<Material> material)
    {
        const uint32_t index = static_cast<uint32_t>(m_materials.size());
        m_materials.push_back(std::move(material));
        return MaterialHandle{ index };
    }

    Mesh* RenderResourceManager::ResolveMesh(MeshHandle handle)
    {
        if (!handle.IsValid() || handle.value >= m_meshes.size())
        {
            return nullptr;
        }

        return m_meshes[handle.value].get();
    }

    Material* RenderResourceManager::ResolveMaterial(MaterialHandle handle)
    {
        if (!handle.IsValid() || handle.value >= m_materials.size())
        {
            return nullptr;
        }

        return m_materials[handle.value].get();
    }
}