cmake_minimum_required(VERSION 3.8)
project(Hyv)

file(GLOB_RECURSE SRC src/*.cpp)
file(GLOB_RECURSE INC include/*.h)

add_subdirectory(lib/DiligentCore)
add_subdirectory(lib/DiligentTools)
add_subdirectory(lib/glm)
add_subdirectory(lib/assimp)
add_subdirectory(lib/fmt)
add_subdirectory(lib/flecs)
add_subdirectory(lib/glfw)
	
add_executable(Hyv ${SRC} ${INC} dev/dev.cpp lib/DiligentTools/ThirdParty/imgui/backends/imgui_impl_glfw.cpp)

target_include_directories(Hyv PRIVATE lib/DiligentCore lib/DiligentTools/TextureLoader/interface lib/DiligentTools/Imgui/interface
lib/DiligentTools/ThirdParty/imgui/backends)

target_link_libraries(Hyv
PRIVATE
    Diligent-GraphicsEngineD3D12-shared
    Diligent-GraphicsEngineVk-shared
    Diligent-Common
    Diligent-GraphicsTools
    Diligent-TextureLoader
    Diligent-Imgui
    fmt
    glm
    assimp::assimp
    glfw
    flecs_static
)
copy_required_dlls(Hyv)

set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT Hyv)

target_compile_features(Hyv PUBLIC cxx_std_20)

target_compile_options(Hyv PRIVATE -DUNICODE -DENGINE_DLL -DRES=\"${CMAKE_SOURCE_DIR}/resources\" -DSHADER_RES=\"${CMAKE_SOURCE_DIR}/resources/shaders\")

target_include_directories(Hyv PUBLIC include)