#include <d3d11.h>
#include <wrl/client.h>

#include <cstdint>

namespace Engine
{
    class GraphicsDevice;

    class RenderTarget final
    {
    public:
        bool InitializeFromBackBuffer(GraphicsDevice& graphicsDevice);
        void Shutdown();

        ID3D11RenderTargetView* GetRenderTargetView() const
        {
            return m_renderTargetView.Get();
        }

    private:
        Microsoft::WRL::ComPtr<ID3D11RenderTargetView> m_renderTargetView;
    };
}