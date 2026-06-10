#include "Engine/Renderer/ShaderProgram.h"

#include "Engine/Core/Log.h"
#include "Engine/Renderer/DxHelpers.h"
#include "Engine/Renderer/GraphicsDevice.h"

#include <d3dcompiler.h>

namespace Engine {
  namespace {
    Microsoft::WRL::ComPtr<ID3DBlob> CompileShader(const wchar_t* shaderPath, const char* entryPoint,
                                                   const char* target) {
      UINT compileFlags = D3DCOMPILE_ENABLE_STRICTNESS;

#if defined(_DEBUG)
      compileFlags |= D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#endif

      Microsoft::WRL::ComPtr<ID3DBlob> shaderBlob;
      Microsoft::WRL::ComPtr<ID3DBlob> errorBlob;

      HRESULT hr = D3DCompileFromFile(shaderPath,
                                      nullptr,
                                      D3D_COMPILE_STANDARD_FILE_INCLUDE,
                                      entryPoint,
                                      target,
                                      compileFlags,
                                      0,
                                      shaderBlob.GetAddressOf(),
                                      errorBlob.GetAddressOf());

      if (FAILED(hr)) {
        if (errorBlob) {
          LogError(static_cast<const char*>(errorBlob->GetBufferPointer()));
        }

        DX_CHECK(hr);
        return nullptr;
      }

      return shaderBlob;
    }
  }

  bool ShaderProgram::Initialize(GraphicsDevice& graphicsDevice, const wchar_t* shaderPath,
                                 const D3D11_INPUT_ELEMENT_DESC* inputElements, unsigned int inputElementCount) {
    Microsoft::WRL::ComPtr<ID3DBlob> vertexShaderBlob = CompileShader(shaderPath, "VSMain", "vs_5_0");

    if (!vertexShaderBlob) {
      return false;
    }

    Microsoft::WRL::ComPtr<ID3DBlob> pixelShaderBlob = CompileShader(shaderPath, "PSMain", "ps_5_0");

    if (!pixelShaderBlob) {
      return false;
    }

    HRESULT hr = graphicsDevice.GetDevice()->CreateVertexShader(vertexShaderBlob->GetBufferPointer(),
                                                                vertexShaderBlob->GetBufferSize(),
                                                                nullptr,
                                                                m_vertexShader.GetAddressOf());

    if (!DX_CHECK(hr)) {
      return false;
    }

    hr = graphicsDevice.GetDevice()->CreatePixelShader(
        pixelShaderBlob->GetBufferPointer(), pixelShaderBlob->GetBufferSize(), nullptr, m_pixelShader.GetAddressOf());

    if (!DX_CHECK(hr)) {
      return false;
    }

    if (inputElements && inputElementCount > 0) {
      hr = graphicsDevice.GetDevice()->CreateInputLayout(inputElements,
                                                         inputElementCount,
                                                         vertexShaderBlob->GetBufferPointer(),
                                                         vertexShaderBlob->GetBufferSize(),
                                                         m_inputLayout.GetAddressOf());

      if (!DX_CHECK(hr)) {
        return false;
      }
    } else {
      m_inputLayout.Reset();
    }

    if (!DX_CHECK(hr)) {
      return false;
    }

    LogInfo("ShaderProgram initialized.");
    return true;
  }

  void ShaderProgram::Shutdown() {
    m_inputLayout.Reset();
    m_pixelShader.Reset();
    m_vertexShader.Reset();
  }

  void ShaderProgram::Bind(GraphicsDevice& graphicsDevice) {
    ID3D11DeviceContext* context = graphicsDevice.GetContext();

    context->IASetInputLayout(m_inputLayout.Get());
    context->VSSetShader(m_vertexShader.Get(), nullptr, 0);
    context->PSSetShader(m_pixelShader.Get(), nullptr, 0);
  }
}