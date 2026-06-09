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
)

target_include_directories(RenderSandbox PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(RenderSandbox PRIVATE
    d3d11
    dxgi
    d3dcompiler
    dxguid
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