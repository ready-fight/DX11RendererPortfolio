add_executable(RenderSandbox WIN32
    Runtime/Win32/Win32Entry.cpp

    Engine/Application/Application.h
    Engine/Application/Application.cpp

    Engine/Core/Assert.h
    Engine/Core/Log.h
    Engine/Core/Log.cpp
    Engine/Core/Timer.h
    Engine/Core/Timer.cpp

    Engine/Platform/Window.h
    Engine/Platform/Win32Window.h
    Engine/Platform/Win32Window.cpp

    Engine/Renderer/GraphicsDevice.h
    Engine/Renderer/GraphicsDevice.cpp
    Engine/Renderer/DxHelpers.h
    Engine/Renderer/DxHelpers.cpp
    Engine/Renderer/VertexTypes.h
    Engine/Renderer/Camera.h
    Engine/Renderer/Camera.cpp
    Engine/Renderer/ShaderProgram.h
    Engine/Renderer/ShaderProgram.cpp
    Engine/Renderer/GpuBuffer.h
    Engine/Renderer/GpuBuffer.cpp
    Engine/Renderer/TrianglePass.h
    Engine/Renderer/TrianglePass.cpp
    Engine/Renderer/MeshPass.h
    Engine/Renderer/MeshPass.cpp
    Engine/Renderer/RenderStates.h
    Engine/Renderer/RenderStates.cpp
    Engine/Renderer/Mesh.h
    Engine/Renderer/Mesh.cpp
    Engine/Renderer/MeshFactory.h
    Engine/Renderer/MeshFactory.cpp
    Engine/Scene/Transform.h
    Engine/Scene/SceneObject.h
    Engine/Renderer/Material.h
    Engine/Renderer/Material.cpp
    Engine/Renderer/Texture2D.h
    Engine/Renderer/Texture2D.cpp
    Engine/Renderer/RenderTarget.h
    Engine/Renderer/RenderTarget.cpp
    Engine/Renderer/DepthStencilBuffer.h
    Engine/Renderer/DepthStencilBuffer.cpp
    Engine/Renderer/FullscreenPass.h
    Engine/Renderer/FullscreenPass.cpp
    Engine/Renderer/PostProcessSettings.h
    Engine/Input/KeyboardInput.h
    Engine/Input/KeyboardInput.cpp
    Engine/Renderer/DebugSettings.h
    Engine/Renderer/DebugOverlay.h
    Engine/Renderer/DebugOverlay.cpp
    Engine/Renderer/RenderStats.h
    Engine/Scene/Scene.h
    Engine/Scene/Scene.cpp
    Engine/Renderer/RenderResourceHandles.h
    Engine/Renderer/RenderResourceManager.h
    Engine/Renderer/RenderResourceManager.cpp
    Engine/Renderer/MaterialInstance.h
    Engine/Renderer/MaterialTypes.h
    Engine/Renderer/MeshData.h
    Engine/Renderer/ObjLoader.h
    Engine/Renderer/ObjLoader.cpp
    Engine/Scene/DirectionalLight.h
    Engine/Renderer/ParticlePass.h
    Engine/Renderer/ParticlePass.cpp
)

target_include_directories(RenderSandbox PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
)

set(IMGUI_DIR ${CMAKE_SOURCE_DIR}/external/imgui)

add_library(imgui STATIC
    ${IMGUI_DIR}/imgui.cpp
    ${IMGUI_DIR}/imgui_draw.cpp
    ${IMGUI_DIR}/imgui_tables.cpp
    ${IMGUI_DIR}/imgui_widgets.cpp
    ${IMGUI_DIR}/backends/imgui_impl_win32.cpp
    ${IMGUI_DIR}/backends/imgui_impl_dx11.cpp
)

target_include_directories(imgui PUBLIC
    ${IMGUI_DIR}
    ${IMGUI_DIR}/backends
)

target_link_libraries(RenderSandbox PRIVATE
    imgui
    d3d11
    dxgi
    d3dcompiler
    dxguid
    windowscodecs
)

target_compile_definitions(RenderSandbox PRIVATE
    WIN32_LEAN_AND_MEAN
    NOMINMAX
    UNICODE
    _UNICODE
)

if (MSVC)
    target_compile_options(RenderSandbox PRIVATE
        /W4
        /permissive-
        /Zc:__cplusplus
    )
endif()