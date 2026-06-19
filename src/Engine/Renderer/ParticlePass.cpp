#include "Engine/Renderer/ParticlePass.h"

#include "Engine/Renderer/Camera.h"
#include "Engine/Renderer/GraphicsDevice.h"

#include <cmath>

namespace Engine
{
    namespace
    {
        struct ParticleConstants
        {
            DirectX::XMFLOAT4X4 viewProjection;

            DirectX::XMFLOAT4 centerSize;
            DirectX::XMFLOAT4 color;

            DirectX::XMFLOAT4 cameraRight;
            DirectX::XMFLOAT4 cameraUp;
        };

        static_assert(sizeof(ParticleConstants) % 16 == 0);
    }

    bool ParticlePass::Initialize(GraphicsDevice& graphicsDevice)
    {
        if (!m_shader.Initialize(
            graphicsDevice,
            L"assets/shaders/AdditiveParticle.hlsl",
            nullptr,
            0))
        {
            return false;
        }

        if (!m_particleBuffer.CreateConstantBuffer(
            graphicsDevice,
            sizeof(ParticleConstants)))
        {
            return false;
        }

        return true;
    }

    void ParticlePass::Shutdown()
    {
        m_particleBuffer.Shutdown();
        m_shader.Shutdown();
    }

    void ParticlePass::Render(
        GraphicsDevice& graphicsDevice,
        const Camera& camera,
        float totalTime)
    {
        using namespace DirectX;

        const XMMATRIX view = camera.GetViewMatrix();
        const XMMATRIX projection = camera.GetProjectionMatrix();
        const XMMATRIX viewProjection = view * projection;

        XMFLOAT4X4 viewProjectionFloat = {};
        XMStoreFloat4x4(&viewProjectionFloat, viewProjection);

        const XMMATRIX inverseView = XMMatrixInverse(nullptr, view);

        XMFLOAT3 cameraRight = {};
        XMFLOAT3 cameraUp = {};

        XMStoreFloat3(&cameraRight, inverseView.r[0]);
        XMStoreFloat3(&cameraUp, inverseView.r[1]);

        ID3D11DeviceContext* context = graphicsDevice.GetContext();

        context->IASetInputLayout(nullptr);
        context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

        ID3D11Buffer* nullVertexBuffers[] = { nullptr };
        UINT nullStrides[] = { 0 };
        UINT nullOffsets[] = { 0 };

        context->IASetVertexBuffers(
            0,
            1,
            nullVertexBuffers,
            nullStrides,
            nullOffsets
        );

        context->IASetIndexBuffer(nullptr, DXGI_FORMAT_R32_UINT, 0);

        m_shader.Bind(graphicsDevice);

        constexpr int particleCount = 12;

        for (int i = 0; i < particleCount; ++i)
        {
            const float normalizedIndex =
                static_cast<float>(i) / static_cast<float>(particleCount);

            const float angle =
                totalTime * 1.2f + normalizedIndex * DirectX::XM_2PI;

            const float radius = 1.35f;

            const float x = std::cos(angle) * radius;
            const float z = std::sin(angle) * radius;
            const float y = 0.15f + std::sin(totalTime * 2.0f + i) * 0.35f;

            const float size =
                0.10f + std::sin(totalTime * 3.0f + i) * 0.025f;

            ParticleConstants constants = {};
            constants.viewProjection = viewProjectionFloat;

            constants.centerSize =
            {
                x,
                y,
                z,
                size
            };

            constants.color =
            {
                0.25f,
                0.75f,
                1.0f,
                0.85f
            };

            constants.cameraRight =
            {
                cameraRight.x,
                cameraRight.y,
                cameraRight.z,
                0.0f
            };

            constants.cameraUp =
            {
                cameraUp.x,
                cameraUp.y,
                cameraUp.z,
                0.0f
            };

            m_particleBuffer.Update(
                graphicsDevice,
                &constants,
                sizeof(constants)
            );

            m_particleBuffer.BindConstantBufferVS(graphicsDevice, 0);
            m_particleBuffer.BindConstantBufferPS(graphicsDevice, 0);

            graphicsDevice.Draw(6, 0);
        }
    }
}